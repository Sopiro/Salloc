#include <crtdbg.h>
#include <iostream>

#include "allocator/fixed_block_allocator.h"

int main(int argc, char** argv)
{
#if defined(_WIN32) && defined(_DEBUG)
    // Enable memory-leak reports
    _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG));
#endif

    struct Vec2
    {
        int32 x, y;
    };

    FixedBlockAllocator<sizeof(Vec2)> fba;
    const int32 count = 100;
    Vec2* k[count];

    for (int32 i = 0; i < count; i++)
    {
        k[i] = (Vec2*)fba.Allocate();
        k[i]->x = i;
        k[i]->y = i;
    }

    std::cout << fba.GetChunkCount() << ", " << fba.GetBlockCount() << std::endl;

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

    std::cout << fba.GetChunkCount() << ", " << fba.GetBlockCount() << std::endl;

    return 0;
}
