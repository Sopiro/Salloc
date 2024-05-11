#pragma once

#include "allocator.h"

namespace salloc
{

// You must nest allocate/free pairs
class LinearAllocator : public Allocator
{
public:
    LinearAllocator(size_t initialCapacity = 16 * 1024);
    ~LinearAllocator();

    virtual void* Allocate(size_t size) override;
    virtual void Free(void* p, size_t size) override;
    virtual void Clear() override;

    bool GrowMemory();

    size_t GetCapacity() const;
    size_t GetAllocation() const;
    size_t GetMaxAllocation() const;

private:
    struct MemoryEntry
    {
        char* data;
        size_t size;
        bool mallocUsed;
    };

    MemoryEntry* entries;
    size_t entryCount;
    size_t entryCapacity;

    char* mem;
    size_t capacity;
    size_t index;

    size_t allocation;
    size_t maxAllocation;
};

inline size_t LinearAllocator::GetCapacity() const
{
    return capacity;
}

inline size_t LinearAllocator::GetAllocation() const
{
    return allocation;
}

inline size_t LinearAllocator::GetMaxAllocation() const
{
    return maxAllocation;
}

} // namespace salloc
