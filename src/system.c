#include "system.h"

#pragma optimize("", off)
void memcl(void* dst, size_t size)
{
    char* realdst = (char*)dst;
    for (size_t i = 0; i < size; i++)
        realdst[i] = 0;
}
#pragma optimize("", on)