#pragma once

#include "common.h"

class Allocator
{
public:
    Allocator() = default;
    virtual ~Allocator() = default;

    virtual void* Allocate(size_t size) = 0;
    virtual void Free(void* p, size_t size) = 0;
    virtual void Clear() = 0;
};
