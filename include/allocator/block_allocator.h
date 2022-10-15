#pragma once

#include "common.h"

constexpr size_t blockSizeCount = 14;

class BlockAllocator
{
public:
    BlockAllocator();
    ~BlockAllocator();

    void* Allocate(size_t size);
    void Free(void* p, size_t size);
    void Clear();

    size_t GetBlockCount() const;
    size_t GetChunkCount() const;

private:
    size_t blockCount;
    size_t chunkCount;

    Chunk* chunks;
    Block* freeList[blockSizeCount];
};

inline size_t BlockAllocator::GetBlockCount() const
{
    return blockCount;
}

inline size_t BlockAllocator::GetChunkCount() const
{
    return chunkCount;
}
