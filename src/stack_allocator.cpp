#include "allocator/stack_allocator.h"

StackAllocator::StackAllocator()
    : index{ 0 }
    , allocation{ 0 }
    , maxAllocation{ 0 }
    , entryCount{ 0 }
{
}

StackAllocator::~StackAllocator()
{
    assert(index == 0 && entryCount == 0);
}

void* StackAllocator::Allocate(size_t size)
{
    assert(entryCount < max_stack_entries && "Increase the max_stack_entries");

    StackEntry* entry = entries + entryCount;
    entry->size = size;

    if (index + size > stack_size)
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

void StackAllocator::Free(void* p, size_t size)
{
    assert(entryCount > 0);

    StackEntry* entry = entries + entryCount - 1;
    assert(entry->data == p);
    assert(entry->size == size * 8);

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
