#include "salloc/block_allocator.h"

namespace salloc
{

BlockAllocator::BlockAllocator(size_t initialChunkSize)
    : blockCount{ 0 }
    , chunkCount{ 0 }
    , chunks{ nullptr }
{
    memset(freeList, 0, sizeof(freeList));

    for (size_t i = 0; i < block_size_count; ++i)
    {
        chunkSizes[i] = initialChunkSize;
    }
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
    if (size > max_block_size)
    {
        return malloc(size);
    }

    assert(0 < size && size <= max_block_size);

    size_t blockSize = size;
    size_t index = size / block_unit;
    size_t mod = size % block_unit;
    if (mod != 0)
    {
        blockSize += block_unit - mod;
    }
    else
    {
        --index;
    }

    assert(0 <= index && index <= block_size_count);

    if (freeList[index] == nullptr)
    {
        // Increase chunk size by half
        chunkSizes[index] += chunkSizes[index] / 2;

        size_t chunkSize = chunkSizes[index];
        size_t blockCapacity = chunkSize / blockSize;

        Block* blocks = (Block*)malloc(chunkSize);

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
        newChunk->capacity = blockCapacity;
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

    if (size > max_block_size)
    {
        free(p);
        return;
    }

    size_t blockSize = size;
    size_t index = size / block_unit;
    size_t mod = size % block_unit;
    if (mod != 0)
    {
        blockSize += block_unit - mod;
    }
    else
    {
        --index;
    }

    assert(0 <= index && index <= block_size_count);

#if defined(_DEBUG)
    // Verify the memory address and size is valid.
    bool found = false;

    Chunk* chunk = chunks;
    while (chunk)
    {
        size_t chunkSize = chunk->capacity * chunk->blockSize;
        if (chunk->blockSize != blockSize)
        {
            assert((char*)p + blockSize <= (char*)chunk->blocks || (char*)chunk->blocks + chunkSize <= (char*)p);
        }
        else
        {
            if (((char*)chunk->blocks <= (char*)p && (char*)p + blockSize <= (char*)chunk->blocks + chunkSize))
            {
                found = true;
                break;
            }
        }

        chunk = chunk->next;
    }

    assert(found);
#else
    sallocNotUsed(blockSize);
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

void BlockAllocator::Clear(size_t initialChunkSize)
{
    Clear();

    for (size_t i = 0; i < block_size_count; ++i)
    {
        chunkSizes[i] = initialChunkSize;
    }
}

size_t BlockAllocator::GetChunkSize(size_t size) const
{
    size_t index = size / block_unit;
    if (size % block_unit == 0)
    {
        --index;
    }

    return chunkSizes[index];
}

} // namespace salloc
