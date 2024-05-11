#pragma once

#include "allocator.h"

namespace salloc
{

// Stack allocator is used for transient, predictable allocations.
// You must nest allocate/free pairs
class StackAllocator : public Allocator
{
public:
    static constexpr inline size_t stack_size = 100 * 1024;
    static constexpr inline size_t max_stack_entries = 32;

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

    char stack[stack_size];
    size_t index;

    size_t allocation;
    size_t maxAllocation;

    StackEntry entries[max_stack_entries];
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

} // namespace salloc
