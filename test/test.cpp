#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "block_allocator.h"
#include "fixed_block_allocator.h"
#include "predefined_block_allocator.h"
#include "stack_allocator.h"

TEST_CASE("Allocators")
{
#if defined(_WIN32) && defined(_DEBUG)
    // Enable memory-leak reports
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
#endif

    struct Vec2
    {
        int32 x, y;
    };

    SUBCASE("Stack allocator")
    {
        StackAllocator sa;

        for (size_t i = 1; i <= maxStackEntries; i++)
        {
            sa.Allocate(i);
        }

        REQUIRE_EQ(sa.GetAllocation(), sa.GetMaxAllocation());
        REQUIRE_EQ(sa.GetAllocation(), (1 + maxStackEntries) * maxStackEntries / 2);

        sa.Clear();
    }

    SUBCASE("Fixed block allocator")
    {
        FixedBlockAllocator<sizeof(Vec2)> fba;
        const int32 count = 100;
        Vec2* k[count];

        for (int32 i = 0; i < count; i++)
        {
            k[i] = (Vec2*)fba.Allocate();
            k[i]->x = i;
            k[i]->y = i;
        }

        size_t chunkCount = fba.GetChunkCount();
        REQUIRE_EQ(fba.GetBlockCount(), count);

        for (int32 i = 0; i < count; i++)
        {
            fba.Free(k[i]);
        }

        for (int32 i = 0; i < count / 2; i++)
        {
            k[i] = (Vec2*)fba.Allocate();
            k[i]->x = i;
            k[i]->y = i;
        }

        REQUIRE_EQ(fba.GetChunkCount(), chunkCount);
        REQUIRE_EQ(fba.GetBlockCount(), count / 2);
    }

    SUBCASE("Block allocator")
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

        REQUIRE_EQ(fdba.GetChunkCount(), predefinedBlockSizeCount);
        REQUIRE_EQ(fdba.GetBlockCount(), predefinedBlockSizeCount);

        // Allocations fit in exsiting memory block
        fdba.Allocate(17);
        fdba.Allocate(18);
        fdba.Allocate(19);

        REQUIRE_EQ(fdba.GetChunkCount(), predefinedBlockSizeCount); // Not changed
        REQUIRE_EQ(fdba.GetBlockCount(), predefinedBlockSizeCount + 3);
    }

    SUBCASE("Block allocator")
    {
        BlockAllocator ba;

        size_t count = maxBlockSize;
        for (size_t i = 1; i <= maxBlockSize; i++)
        {
            ba.Allocate(i);
        }

        REQUIRE_EQ(ba.GetChunkCount(), maxBlockSize / blockUnit);
        REQUIRE_EQ(ba.GetBlockCount(), count);
    }
}
