#pragma once

#include "allocator.h"

constexpr size_t max_block_size = 1024;
constexpr size_t block_unit = 8;
constexpr size_t block_size_count = max_block_size / block_unit;

class BlockAllocator : public Allocator
{
public:
    BlockAllocator();
    ~BlockAllocator();

    virtual void* Allocate(size_t size) override;
    virtual void Free(void* p, size_t size) override;
    virtual void Clear() override;

    size_t GetBlockCount() const;
    size_t GetChunkCount() const;

private:
    size_t blockCount;
    size_t chunkCount;

    Chunk* chunks;
    Block* freeList[block_size_count];
};

inline size_t BlockAllocator::GetBlockCount() const
{
    return blockCount;
}

inline size_t BlockAllocator::GetChunkCount() const
{
    return chunkCount;
}
