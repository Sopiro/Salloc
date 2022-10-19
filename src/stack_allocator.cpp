#include "allocator/stack_allocator.h"

StackAllocator::StackAllocator()
{
    index = 0;
    allocation = 0;
    maxAllocation = 0;
    entryCount = 0;
}

StackAllocator::~StackAllocator()
{
    assert(index == 0 && entryCount == 0);
}

void* StackAllocator::Allocate(size_t size)
{
    assert(entryCount < maxStackEntries && "Increase the maxStackEntries");

    StackEntry* entry = entries + entryCount;
    entry->size = size;

    if (index + size > stackSize)
    {
        entry->data = (int8*)malloc(size);
        entry->mallocUsed = true;
    }
    else
    {
        entry->data = (int8*)stack + index;
        entry->mallocUsed = false;
        index += size;
    }

    allocation += size;
    maxAllocation = Max(maxAllocation, allocation);
    ++entryCount;

    return entry->data;
}

void StackAllocator::Free(void* p)
{
    assert(entryCount > 0);

    StackEntry* entry = entries + entryCount - 1;
    assert(entry->data == p);

    if (entry->mallocUsed)
    {
        free(p);
    }
    else
    {
        index -= entry->size;
    }

    allocation -= entry->size;
    --entryCount;

    p = nullptr;
}

void StackAllocator::Clear()
{
    index = 0;
    allocation = 0;
    maxAllocation = 0;
    entryCount = 0;
}
