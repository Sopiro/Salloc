#pragma once

#include "allocator.h"

namespace salloc
{

class PredefinedBlockAllocator : public Allocator
{
public:
    static constexpr inline size_t block_size_count = 14;

    PredefinedBlockAllocator();
    ~PredefinedBlockAllocator();

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

inline size_t PredefinedBlockAllocator::GetBlockCount() const
{
    return blockCount;
}

inline size_t PredefinedBlockAllocator::GetChunkCount() const
{
    return chunkCount;
}

} // namespace salloc