#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "block_allocator.h"
#include "fixed_block_allocator.h"
#include "linear_allocator.h"
#include "predefined_block_allocator.h"
#include "stack_allocator.h"

using namespace salloc;

TEST_CASE("Allocators")
{
#if defined(_WIN32) && defined(_DEBUG)
    // Enable memory-leak reports
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
#endif
}

TEST_CASE("Stack allocator")
{
    constexpr size_t stackSize = 100 * 1024;
    constexpr size_t maxStackEntries = 32;
    StackAllocator<stackSize, maxStackEntries> sa;

    for (size_t i = 1; i <= maxStackEntries; i++)
    {
        sa.Allocate(i);
    }

    REQUIRE_EQ(sa.GetAllocation(), sa.GetMaxAllocation());
    REQUIRE_EQ(sa.GetAllocation(), (1 + maxStackEntries) * maxStackEntries / 2);

    sa.Clear();
}

TEST_CASE("Linear allocator")
{
    LinearAllocator la;

    size_t cap = la.GetCapacity();
    REQUIRE_EQ(la.GrowMemory(), false);
    REQUIRE_EQ(la.GetCapacity(), cap);

    void* m = la.Allocate(cap);
    la.Free(m, cap);

    REQUIRE_EQ(la.GetAllocation(), 0);
    REQUIRE_EQ(la.GetMaxAllocation(), cap);

    REQUIRE_EQ(la.GrowMemory(), true);

    REQUIRE_EQ(la.GetCapacity(), cap + cap / 2);

    la.Clear();
}

TEST_CASE("Fixed block allocator")
{
    struct Vec2
    {
        int x, y;
    };

    FixedBlockAllocator<sizeof(Vec2)> fba;
    const int count = 100;
    Vec2* k[count];

    for (int i = 0; i < count; i++)
    {
        k[i] = (Vec2*)fba.Allocate();
        k[i]->x = i;
        k[i]->y = i;
    }

    size_t chunkCount = fba.GetChunkCount();
    REQUIRE_EQ(fba.GetBlockCount(), count);

    for (int i = 0; i < count; i++)
    {
        fba.Free(k[i]);
    }

    for (int i = 0; i < count / 2; i++)
    {
        k[i] = (Vec2*)fba.Allocate();
        k[i]->x = i;
        k[i]->y = i;
    }

    REQUIRE_EQ(fba.GetChunkCount(), chunkCount);
    REQUIRE_EQ(fba.GetBlockCount(), count / 2);
}

TEST_CASE("Predefined block allocator")
{
    size_t blockSizes[14] = { 16, 32, 64, 96, 128, 160, 192, 224, 256, 320, 384, 448, 512, 640 };

    PredefinedBlockAllocator pba(16 * 1024, blockSizes);

    // Allocate with predefined block sizes
    pba.Allocate(16);
    pba.Allocate(32);
    pba.Allocate(64);
    pba.Allocate(96);
    pba.Allocate(128);
    pba.Allocate(160);
    pba.Allocate(192);
    pba.Allocate(224);
    pba.Allocate(256);
    pba.Allocate(320);
    pba.Allocate(384);
    pba.Allocate(448);
    pba.Allocate(512);
    pba.Allocate(640);

    REQUIRE_EQ(pba.GetChunkCount(), pba.GetBlockSizeCount());
    REQUIRE_EQ(pba.GetBlockCount(), pba.GetBlockSizeCount());

    // Allocations fit in exsiting memory block
    pba.Allocate(17);
    pba.Allocate(18);
    pba.Allocate(19);

    REQUIRE_EQ(pba.GetChunkCount(), pba.GetBlockSizeCount()); // Not changed
    REQUIRE_EQ(pba.GetBlockCount(), pba.GetBlockSizeCount() + 3);
}

TEST_CASE("Block allocator")
{
    BlockAllocator ba;

    size_t count = BlockAllocator::max_block_size;
    for (size_t i = 1; i <= BlockAllocator::max_block_size; i++)
    {
        ba.Allocate(i);
    }

    REQUIRE_EQ(ba.GetChunkCount(), BlockAllocator::block_size_count);
    REQUIRE_EQ(ba.GetBlockCount(), count);

    size_t chunkSize1 = ba.GetChunkSize(1);

    // Check for chunk size growing
    for (size_t i = 1; i <= BlockAllocator::max_block_size; i++)
    {
        size_t c = ba.GetChunkSize(i);
        REQUIRE_EQ(chunkSize1, c);
    }

    ba.Clear();

    for (size_t i = 0; i < chunkSize1 / BlockAllocator::block_unit; i++)
    {
        ba.Allocate(1);
    }

    REQUIRE_EQ(ba.GetChunkSize(1), chunkSize1 + chunkSize1 / 2);
    REQUIRE_EQ(ba.GetChunkSize(2), chunkSize1 + chunkSize1 / 2);
    REQUIRE_EQ(ba.GetChunkSize(4), chunkSize1 + chunkSize1 / 2);
    REQUIRE_EQ(ba.GetChunkSize(8), chunkSize1 + chunkSize1 / 2);

    REQUIRE_EQ(ba.GetChunkSize(9), chunkSize1);
}

TEST_CASE("New Delete")
{
    BlockAllocator ba;

    struct Vec2
    {
        int x, y;

        Vec2(int x, int y)
            : x{ x }
            , y{ y }
        {
            std::cout << "Vec2 created: " << x << ", " << y << std::endl;
        }

        ~Vec2()
        {
            std::cout << "Vec2 deleted: " << x << ", " << y << std::endl;
        }
    };

    Vec2* v = ba.New<Vec2>(12, 34);
    v->x = 56;
    v->y = 78;

    REQUIRE_EQ(ba.GetBlockCount(), 1);
    REQUIRE_EQ(ba.GetChunkCount(), 1);

    ba.Delete(v);

    REQUIRE_EQ(ba.GetBlockCount(), 0);
    REQUIRE_EQ(ba.GetChunkCount(), 1);

    ba.Clear();

    REQUIRE_EQ(ba.GetChunkCount(), 0);
}
