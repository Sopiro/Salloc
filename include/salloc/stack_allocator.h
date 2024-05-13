#pragma once

#include "allocator.h"

namespace salloc
{

// Stack allocator is used for transient, predictable allocations.
// You must nest allocate/free pairs
template <size_t stackSize = 100 * 1024, size_t maxStackEntries = 32>
class StackAllocator : public Allocator
{
public:
    StackAllocator();
    ~StackAllocator();

    virtual void* Allocate(size_t size) override;
    virtual void Free(void* p, size_t size) override;
    virtual void Clear() override;

    size_t GetAllocation() const;
    size_t GetMaxAllocation() const;

private:
    struct StackEntry
    {
        char* data;
        size_t size;
        bool mallocUsed;
    };

    char stack[stackSize];
    size_t index;

    size_t allocation;
    size_t maxAllocation;

    StackEntry entries[maxStackEntries];
    size_t entryCount;
};

template <size_t stackSize, size_t maxStackEntries>
StackAllocator<stackSize, maxStackEntries>::StackAllocator()
    : index{ 0 }
    , allocation{ 0 }
    , maxAllocation{ 0 }
    , entryCount{ 0 }
{
}

template <size_t stackSize, size_t maxStackEntries>
StackAllocator<stackSize, maxStackEntries>::~StackAllocator()
{
    assert(index == 0 && entryCount == 0);
}

template <size_t stackSize, size_t maxStackEntries>
void* StackAllocator<stackSize, maxStackEntries>::Allocate(size_t size)
{
    assert(entryCount < maxStackEntries && "Increase the maxStackEntries");

    StackEntry* entry = entries + entryCount;
    entry->size = size;

    if (index + size > stackSize)
    {
        entry->data = (char*)salloc::Alloc(size);
        entry->mallocUsed = true;
    }
    else
    {
        entry->data = stack + index;
        entry->mallocUsed = false;
        index += size;
    }

    allocation += size;
    if (allocation > maxAllocation)
    {
        maxAllocation = allocation;
    }

    ++entryCount;

    return entry->data;
}

template <size_t stackSize, size_t maxStackEntries>
void StackAllocator<stackSize, maxStackEntries>::Free(void* p, size_t size)
{
    sallocNotUsed(size);
    assert(entryCount > 0);

    StackEntry* entry = entries + (entryCount - 1);
    assert(entry->data == p);
    assert(entry->size == size);

    if (entry->mallocUsed)
    {
        salloc::Free(p);
    }
    else
    {
        index -= entry->size;
    }

    allocation -= entry->size;
    --entryCount;

    p = nullptr;
}

template <size_t stackSize, size_t maxStackEntries>
void StackAllocator<stackSize, maxStackEntries>::Clear()
{
    index = 0;
    allocation = 0;
    maxAllocation = 0;
    entryCount = 0;
}

template <size_t stackSize, size_t maxStackEntries>
size_t StackAllocator<stackSize, maxStackEntries>::GetAllocation() const
{
    return allocation;
}

template <size_t stackSize, size_t maxStackEntries>
size_t StackAllocator<stackSize, maxStackEntries>::GetMaxAllocation() const
{
    return maxAllocation;
}

} // namespace salloc
