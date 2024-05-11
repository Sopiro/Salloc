#include "salloc/predefined_block_allocator.h"

namespace salloc
{

// Predefined block sizes
static constexpr size_t block_sizes[PredefinedBlockAllocator::block_size_count] = {
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

static constexpr size_t chunk_size = 16 * 1024;
static constexpr size_t max_block_size = block_sizes[PredefinedBlockAllocator::block_size_count - 1];

struct SizeMap
{
    SizeMap()
    {
        size_t j = 0;
        values[0] = 0;
        for (size_t i = 1; i <= max_block_size; ++i)
        {
            if (i <= block_sizes[j])
            {
                values[i] = j;
            }
            else
            {
                ++j;
                values[i] = j;
            }
        }
    }

    size_t values[max_block_size + 1];
};

static const SizeMap size_map;

PredefinedBlockAllocator::PredefinedBlockAllocator()
    : blockCount{ 0 }
    , chunkCount{ 0 }
    , chunks{ nullptr }
{
    memset(freeList, 0, sizeof(freeList));
}

PredefinedBlockAllocator::~PredefinedBlockAllocator()
{
    Clear();
}

void* PredefinedBlockAllocator::Allocate(size_t size)
{
    if (size == 0)
    {
        return nullptr;
    }
    if (size > max_block_size)
    {
        // assert(false);
        return malloc(size);
    }

    size_t index = size_map.values[size];
    assert(0 <= index && index <= block_size_count);

    if (freeList[index] == nullptr)
    {
        Block* blocks = (Block*)malloc(chunk_size);
        size_t blockSize = block_sizes[index];
        size_t blockCapacity = chunk_size / blockSize;

        // Build a linked list for the free list.
        for (size_t i = 0; i < blockCapacity - 1; ++i)
        {
            Block* block = (Block*)((char*)blocks + blockSize * i);
            Block* next = (Block*)((char*)blocks + blockSize * (i + 1));
            block->next = next;
        }
        Block* last = (Block*)((char*)blocks + blockSize * (blockCapacity - 1));
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

void PredefinedBlockAllocator::Free(void* p, size_t size)
{
    if (size == 0)
    {
        return;
    }

    if (size > max_block_size)
    {
        free(p);
        return;
    }

    assert(0 < size && size <= max_block_size);

    size_t index = size_map.values[size];
    assert(0 <= index && index <= block_size_count);

#if defined(_DEBUG)
    // Verify the memory address and size is valid.
    size_t blockSize = block_sizes[index];
    bool found = false;

    Chunk* chunk = chunks;
    while (chunk)
    {
        if (chunk->blockSize != blockSize)
        {
            assert((char*)p + blockSize <= (char*)chunk->blocks || (char*)chunk->blocks + chunk_size <= (char*)p);
        }
        else
        {
            if (((char*)chunk->blocks <= (char*)p && (char*)p + blockSize <= (char*)chunk->blocks + chunk_size))
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

void PredefinedBlockAllocator::Clear()
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

} // namespace salloc
