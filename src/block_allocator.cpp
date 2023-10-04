#include "allocator/block_allocator.h"

BlockAllocator::BlockAllocator()
    : blockCount{ 0 }
    , chunkCount{ 0 }
    , chunks{ nullptr }
{
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
    size_t blockCapacity = chunk_size / blockSize;

    assert(0 <= index && index <= block_size_count);

    if (freeList[index] == nullptr)
    {
        Block* blocks = (Block*)malloc(chunk_size);

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
