#include "allocator/block_allocator.h"

static constexpr size_t blockSizes[blockSizeCount] = {
    16,  // 0
    32,  // 1
    64,  // 2
    96,  // 3
    128, // 4
    160, // 5
    192, // 6
    224, // 7
    256, // 8
    320, // 9
    384, // 10
    448, // 11
    512, // 12
    640, // 13
};

static constexpr size_t chunkSize = 16 * 1024;
static constexpr size_t maxBlockSize = blockSizes[blockSizeCount - 1];

struct SizeMap
{
    SizeMap()
    {
        size_t j = 0;
        values[0] = 0;
        for (size_t i = 1; i <= maxBlockSize; i++)
        {
            if (i <= blockSizes[j])
            {
                values[i] = j;
            }
            else
            {
                ++j;
                values[i] = j;
            }
        }

        // for (size_t i = 0; i < maxBlockSize + 1; i++)
        // {
        //     std::cout << i << ": " << values[i] << std::endl;
        // }
    }

    size_t values[maxBlockSize + 1];
};

static const SizeMap sizeMap;

BlockAllocator::BlockAllocator()
{
    blockCount = 0;
    chunkCount = 0;
    chunks = nullptr;
    memset(freeList, 0, sizeof(freeList));
}

BlockAllocator::~BlockAllocator()
{
    Clear();
}

void* BlockAllocator::Allocate(size_t size)
{
    if (size == 0)
    {
        return nullptr;
    }
    if (size > maxBlockSize)
    {
        return malloc(size);
    }

    size_t index = sizeMap.values[size];
    assert(0 <= index && index <= blockSizeCount);

    if (freeList[index] == nullptr)
    {
        Block* blocks = (Block*)malloc(chunkSize);
        size_t blockSize = blockSizes[index];
        size_t blockCapacity = chunkSize / blockSize;

        // Build a linked list for the free list.
        for (size_t i = 0; i < blockCapacity - 1; ++i)
        {
            Block* block = (Block*)((int8*)blocks + blockSize * i);
            Block* next = (Block*)((int8*)blocks + blockSize * (i + 1));
            block->next = next;
        }
        Block* last = (Block*)((int8*)blocks + blockSize * (blockCapacity - 1));
        last->next = nullptr;

        Chunk* newChunk = (Chunk*)malloc(sizeof(Chunk));
        newChunk->blockSize = blockSize;
        newChunk->blocks = blocks;
        newChunk->next = chunks;
        chunks = newChunk;
        ++chunkCount;

        freeList[index] = newChunk->blocks;
    }

    Block* block = freeList[index];
    freeList[index] = block->next;
    ++blockCount;

    return block;
}

void BlockAllocator::Free(void* p, size_t size)
{
    if (size == 0)
    {
        return;
    }

    if (size > maxBlockSize)
    {
        free(p);
        return;
    }

    size_t index = sizeMap.values[size];
    assert(0 <= index && index <= blockSizeCount);

#if defined(_DEBUG)
    // Verify the memory address and size is valid.
    size_t blockSize = blockSizes[index];
    bool found = false;

    Chunk* chunk = chunks;
    while (chunk)
    {
        if (chunk->blockSize != blockSize)
        {
            assert((int8*)p + blockSize <= (int8*)chunk->blocks || (int8*)chunk->blocks + chunkSize <= (int8*)p);
        }
        else
        {
            if (((int8*)chunk->blocks <= (int8*)p && (int8*)p + blockSize <= (int8*)chunk->blocks + chunkSize))
            {
                found = true;
                break;
            }
        }

        chunk = chunk->next;
    }

    assert(found);
#endif

    Block* block = (Block*)p;
    block->next = freeList[index];
    freeList[index] = block;
    --blockCount;
}

void BlockAllocator::Clear()
{
    Chunk* chunk = chunks;
    while (chunk)
    {
        Chunk* c0 = chunk;
        chunk = c0->next;
        free(c0->blocks);
        free(c0);
    }

    blockCount = 0;
    chunkCount = 0;
    chunks = nullptr;
    memset(freeList, 0, sizeof(freeList));
}
