#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "block_allocator.h"
#include "fixed_block_allocator.h"
#include "linear_allocator.h"
#include "predefined_block_allocator.h"
#include "stack_allocator.h"

using namespace salloc;

struct Vec2
{
    int x, y;
};

TEST_CASE("Allocators")
{
#if defined(_WIN32) && defined(_DEBUG)
    // Enable memory-leak reports
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
#endif
}

TEST_CASE("Stack allocator")
{
    StackAllocator sa;

    for (size_t i = 1; i <= StackAllocator::max_stack_entries; i++)
    {
        sa.Allocate(i);
    }

    REQUIRE_EQ(sa.GetAllocation(), sa.GetMaxAllocation());
    REQUIRE_EQ(sa.GetAllocation(), (1 + StackAllocator::max_stack_entries) * StackAllocator::max_stack_entries / 2);

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

TEST_CASE("Block allocator")
{
    PredefinedBlockAllocator fdba;

    // Allocate with predefined block sizes
    fdba.Allocate(16);
    fdba.Allocate(32);
    fdba.Allocate(64);
    fdba.Allocate(96);
    fdba.Allocate(128);
    fdba.Allocate(160);
    fdba.Allocate(192);
    fdba.Allocate(224);
    fdba.Allocate(256);
    fdba.Allocate(320);
    fdba.Allocate(384);
    fdba.Allocate(448);
    fdba.Allocate(512);
    fdba.Allocate(640);

    REQUIRE_EQ(fdba.GetChunkCount(), PredefinedBlockAllocator::block_size_count);
    REQUIRE_EQ(fdba.GetBlockCount(), PredefinedBlockAllocator::block_size_count);

    // Allocations fit in exsiting memory block
    fdba.Allocate(17);
    fdba.Allocate(18);
    fdba.Allocate(19);

    REQUIRE_EQ(fdba.GetChunkCount(), PredefinedBlockAllocator::block_size_count); // Not changed
    REQUIRE_EQ(fdba.GetBlockCount(), PredefinedBlockAllocator::block_size_count + 3);
}

TEST_CASE("Block allocator")
{
    BlockAllocator ba;

    size_t count = BlockAllocator::max_block_size;
    for (size_t i = 1; i <= BlockAllocator::max_block_size; i++)
    {
        ba.Allocate(i);
    }

    REQUIRE_EQ(ba.GetChunkCount(), BlockAllocator::max_block_size / BlockAllocator::block_unit);
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
