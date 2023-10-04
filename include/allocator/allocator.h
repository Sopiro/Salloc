#pragma once

#include <assert.h>
#include <string>

struct Block
{
    Block* next;
};

struct Chunk
{
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
};
