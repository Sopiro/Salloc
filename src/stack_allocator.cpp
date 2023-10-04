#include "allocator/stack_allocator.h"

#include <algorithm>

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
        entry->data = (char*)malloc(size);
        entry->mallocUsed = true;
    }
    else
    {
        entry->data = stack + index;
        entry->mallocUsed = false;
        index += size;
    }

    allocation += size;
    maxAllocation = std::max(maxAllocation, allocation);
    ++entryCount;

    return entry->data;
}

void StackAllocator::Free(void* p, size_t size)
{
    assert(entryCount > 0);

    StackEntry* entry = entries + entryCount - 1;
    assert(entry->data == p);
    assert(entry->size == size);

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
