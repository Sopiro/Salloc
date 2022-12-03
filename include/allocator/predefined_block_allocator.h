#pragma once

#include "allocator.h"
#include "common.h"

constexpr size_t predefinedBlockSizeCount = 14;

class PredefinedBlockAllocator : public Allocator
{
public:
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
    Block* freeList[predefinedBlockSizeCount];
};

inline size_t PredefinedBlockAllocator::GetBlockCount() const
{
    return blockCount;
}

inline size_t PredefinedBlockAllocator::GetChunkCount() const
{
    return chunkCount;
}
