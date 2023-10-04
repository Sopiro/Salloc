#pragma once

#include "allocator.h"

class BlockAllocator : public Allocator
{
public:
    static constexpr inline size_t chunk_size = 16 * 1024;

    static constexpr inline size_t max_block_size = 1024;
    static constexpr inline size_t block_unit = 8;
    static constexpr inline size_t block_size_count = max_block_size / block_unit;

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
