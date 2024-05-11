#pragma once

#include "allocator.h"

namespace salloc
{

template <size_t blockSize>
class FixedBlockAllocator : public Allocator
{
public:
    FixedBlockAllocator(size_t initialBlockCapacity = 64)
        : blockCapacity{ initialBlockCapacity }
        , blockCount{ 0 }
        , chunkCount{ 0 }
        , chunks{ nullptr }
        , freeList{ nullptr }
    {
    }

    ~FixedBlockAllocator()
    {
        Clear();
    }

    void* Allocate(size_t size = blockSize) override
    {
        assert(size == blockSize);

        if (size > blockSize)
        {
            return malloc(size);
        }

        if (freeList == nullptr)
        {
            assert(blockCount == 0 || blockCapacity == blockCount / chunkCount);

            blockCapacity += blockCapacity / 2;
            Block* blocks = (Block*)malloc(blockCapacity * blockSize);
            memset(blocks, 0, blockCapacity * blockSize);

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

            freeList = newChunk->blocks;
        }

        void* block = freeList;
        freeList = freeList->next;
        ++blockCount;

        return block;
    }

    void Free(void* p, size_t size = blockSize) override
    {
        assert(0 < blockCount && 0 < chunkCount);

        if (size > blockSize)
        {
            free(p);
            return;
        }

#if defined(_DEBUG)
        // Verify the memory address and size is valid.
        bool found = false;

        Chunk* chunk = chunks;
        while (chunk)
        {
            if ((char*)chunk->blocks <= (char*)p && (char*)p + blockSize <= (char*)chunk->blocks + blockSize * chunk->capacity)
            {
                found = true;
                break;
            }
            chunk = chunk->next;
        }

        assert(found);
#endif

        Block* block = (Block*)p;
        block->next = freeList;
        freeList = block;
        --blockCount;
    }

    void Clear() override
    {
        Chunk* chunk = chunks;
        while (chunk)
        {
            Chunk* c0 = chunk;
            chunk = c0->next;
            free(c0->blocks);
            free(c0);
        }

        chunks = nullptr;
        freeList = nullptr;
    }

    size_t GetChunkCount() const
    {
        return chunkCount;
    }

    size_t GetBlockCount() const
    {
        return blockCount;
    }

private:
    size_t blockCapacity;
    size_t chunkCount;
    size_t blockCount;
    Chunk* chunks;
    Block* freeList;
};

} // namespace salloc
