#include "salloc/linear_allocator.h"

namespace salloc
{

LinearAllocator::LinearAllocator(size_t initialCapacity)
    : entryCount{ 0 }
    , entryCapacity{ 32 }
    , capacity{ initialCapacity }
    , index{ 0 }
    , allocation{ 0 }
    , maxAllocation{ 0 }
{
    mem = (char*)malloc(capacity);
    memset(mem, 0, capacity);
    entries = (MemoryEntry*)malloc(entryCapacity * sizeof(MemoryEntry));
}

LinearAllocator::~LinearAllocator()
{
    assert(index == 0 && entryCount == 0);

    free(entries);
    free(mem);
}

void* LinearAllocator::Allocate(size_t size)
{
    if (entryCount == entryCapacity)
    {
        // Grow entry array by half
        MemoryEntry* old = entries;
        entryCapacity += entryCapacity / 2;
        entries = (MemoryEntry*)malloc(entryCapacity * sizeof(MemoryEntry));
        memcpy(entries, old, entryCount * sizeof(MemoryEntry));
        memset(entries + entryCount, 0, (entryCapacity - entryCount) * sizeof(MemoryEntry));
        free(old);
    }

    MemoryEntry* entry = entries + entryCount;
    entry->size = size;

    if (index + size > capacity)
    {
        entry->data = (char*)malloc(size);
        entry->mallocUsed = true;
    }
    else
    {
        entry->data = mem + index;
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

void LinearAllocator::Free(void* p, size_t size)
{
    sallocNotUsed(size);
    assert(entryCount > 0);

    MemoryEntry* entry = entries + (entryCount - 1);
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

bool LinearAllocator::GrowMemory()
{
    assert(index == 0);

    if (maxAllocation < capacity)
    {
        return false;
    }

    // Grow memory by half
    free(mem);
    capacity += capacity / 2;
    mem = (char*)malloc(capacity);
    memset(mem, 0, capacity);

    return true;
}

void LinearAllocator::Clear()
{
    index = 0;
    entryCount = 0;
    entryCapacity = 0;
    allocation = 0;
    maxAllocation = 0;
}

} // namespace salloc
