#pragma once

#include "allocator.h"

#include <span>
#include <vector>

namespace salloc
{

class PredefinedBlockAllocator : public Allocator
{
    static inline size_t default_block_sizes[14] = {
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

public:
    PredefinedBlockAllocator(size_t initialChunkSize = 16 * 1024, std::span<size_t> blockSizes = default_block_sizes);
    ~PredefinedBlockAllocator();

    virtual void* Allocate(size_t size) override;
    virtual void Free(void* p, size_t size) override;
    virtual void Clear() override;

    size_t GetBlockCount() const;
    size_t GetChunkCount() const;

    size_t GetBlockSizeCount() const;

private:
    struct SizeMap
    {
        SizeMap(std::span<size_t> blockSizes)
            : sizes(blockSizes.begin(), blockSizes.end())
            , values(blockSizes.back() + 1)
        {
            size_t j = 0;
            values[0] = 0;
            size_t maxBlockSize = sizes.back();
            for (size_t i = 1; i <= maxBlockSize; ++i)
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
        }

        size_t MaxBlockSize() const
        {
            return sizes.back();
        }

        std::vector<size_t> sizes, values;
    };

    SizeMap sizeMap;

    size_t blockCount;
    size_t chunkCount;

    size_t chunkSize;
    Chunk* chunks;
    Block** freeList;
};

inline size_t PredefinedBlockAllocator::GetBlockCount() const
{
    return blockCount;
}

inline size_t PredefinedBlockAllocator::GetChunkCount() const
{
    return chunkCount;
}

inline size_t PredefinedBlockAllocator::GetBlockSizeCount() const
{
    return sizeMap.sizes.size();
}

} // namespace salloc