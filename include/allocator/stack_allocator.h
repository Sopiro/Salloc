#pragma once

#include "common.h"

constexpr int32 stackSize = 100 * 1024;
constexpr int32 maxStackEntries = 32;

struct StackEntry
{
    int8* data;
    size_t size;
    bool mallocUsed;
};

// Stack allocator used for transient, predictable allocations.
// You muse nest allocate/free pairs
class StackAllocator
{
public:
    StackAllocator();
    ~StackAllocator();

    void* Allocate(size_t size);
    void Free(void* p);
    void Clear();

    size_t GetAllocation() const;
    size_t GetMaxAllocation() const;

private:
    int8* stack[stackSize];
    size_t index;

    size_t allocation;
    size_t maxAllocation;

    StackEntry entries[maxStackEntries];
    size_t entryCount;
};

inline size_t StackAllocator::GetAllocation() const
{
    return allocation;
}

inline size_t StackAllocator::GetMaxAllocation() const
{
    return maxAllocation;
}