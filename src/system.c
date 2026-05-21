#include "system.h"

#pragma optimize("", off)
void memcl(void* dst, size_t size)
{
    char* realdst = (char*)dst;
    for (size_t i = 0; i < size; i++)
        realdst[i] = 0;
}

void memmv(void* dst, const void* src, size_t size)
{
    char* _dst = (char*)dst;
    const char* _src = (char*)src;
    for (size_t i = 0; i < size; i++)
        _dst[i] = _src[i];
}
#pragma optimize("", on)