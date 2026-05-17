#include "system.h"

void* memset(void* dst, int val, size_t size)
{
    char* realdst = (char*)dst;
    for (size_t i = 0; i < size; i++)
        realdst[i] = (char)val;
    return dst;
}

void* memcpy(void* dst, const void* src, size_t size)
{
    char* _dst = (char*)dst;
    const char* _src = (char*)src;
    for (size_t i = 0; i < size; i++)
        _dst[i] = _src[i];
    return dst;
}