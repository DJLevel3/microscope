#include "system.h"

#ifdef STANDALONE
#pragma optimize("", off)
void memcl(void* dst, int val, size_t size)
{
    char* realdst = (char*)dst;
    for (size_t i = 0; i < size; i++)
        realdst[i] = 0;
}
#pragma optimize("", on)
#else
void mvp43(float* matrix, float* src, float* dest)
{
    float result[4] = { 0, 0, 0, 0 };
    for (int i = 0; i < 4; i++) {
        result[i] = src[0] * matrix[i * 4 + 0] + src[1] * matrix[i * 4 + 1] + src[2] * matrix[i * 4 + 2] + matrix[i * 4 + 3];
    }
    dest[0] = result[0] / result[3];
    dest[1] = result[1] / result[3];
    dest[2] = result[2] / result[3];
}
#pragma optimize("", off)
void* memcl(void* dst, int val, size_t size)
{
    char* realdst = (char*)dst;
    for (size_t i = 0; i < size; i++)
        realdst[i] = (char)val;
    return dst;
}

void* memmv(void* dst, const void* src, size_t size)
{
    char* _dst = (char*)dst;
    const char* _src = (char*)src;
    for (size_t i = 0; i < size; i++)
        _dst[i] = _src[i];
    return dst;
}
#pragma optimize("", on)
#endif