#pragma once

#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

template <typename T>
inline T Max(T a, T b)
{
    return a > b ? a : b;
}
