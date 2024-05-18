#pragma once

#include <assert.h>
#include <cstdlib>
#include <cstring>

#define sallocNotUsed(x) ((void)(x));

namespace salloc
{

// Default upstream allocation function
inline void* Alloc(size_t size)
{
    return std::malloc(size);
}

inline void Free(void* mem)
{
    std::free(mem);
}

struct Block
{
    Block* next;
};

struct Chunk
{
    size_t capacity;
    size_t blockSize;
    Block* blocks;
    Chunk* next;
};

class Allocator
{
public:
    Allocator() = default;
    virtual ~Allocator() = default;

    virtual void* Allocate(size_t size) = 0;
    virtual void Free(void* p, size_t size) = 0;
    virtual void Clear() = 0;

    template <typename T, typename... Args>
    T* New(Args&&... args)
    {
        return new (Allocate(sizeof(T))) T(std::forward<Args>(args)...);
    }

    template <typename T>
    void Delete(T* ptr)
    {
        ptr->~T();
        Free(ptr, sizeof(T));
    }
};

} // namespace salloc
