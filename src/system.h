//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2021                                   //
//--------------------------------------------------------------------------//

#ifndef _SYSTEM_H_
#define _SYSTEM_H_
#include "config.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <GL/GL.h>
#include "glext.h"

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdbool.h>
#include "fp.h"

#define M_PI_F 3.14159265358979323846f

static inline int f2i(float x) // use this to convert float to int, or use /QIfist as additional compiler parameters
{
    int tmp;
    _asm fld dword ptr[x]
        _asm fistp dword ptr[tmp];
    return tmp;
}

extern void *myglfunc[];

#define oglCreateShaderProgramv         ((PFNGLCREATESHADERPROGRAMVPROC)myglfunc[0])
#define oglGenProgramPipelines          ((PFNGLGENPROGRAMPIPELINESPROC)myglfunc[1])
#define oglBindProgramPipeline          ((PFNGLBINDPROGRAMPIPELINEPROC)myglfunc[2])
#define oglUseProgramStages             ((PFNGLUSEPROGRAMSTAGESPROC)myglfunc[3])
#define oglProgramUniform1ui            ((PFNGLPROGRAMUNIFORM1UIPROC)myglfunc[4])
#define oglActiveTexture                ((PFNGLACTIVETEXTUREPROC)myglfunc[5])

#ifdef _DEBUG
#define oglGetProgramiv          ((PFNGLGETPROGRAMIVPROC)myglfunc[6])
#define oglGetProgramInfoLog     ((PFNGLGETPROGRAMINFOLOGPROC)myglfunc[7])
#endif

#ifdef STANDALONE
void memcl(void* mem, int val, size_t ct);
#else

static inline short demo_rand(int* seed)
{
    seed[0] = seed[0] * 0x343FD + 0x269EC3;
    return seed[0] >> 6;
}

void* memcl(void* dst, int val, size_t size);
void* memmv(void* dst, const void* src, size_t size);

void mvp43(float* matrix, float* src, float* dest);

static float view_matrix[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, -5,
    0, 0, 0, 1
};
static float proj_matrix[16] = {
    1.0f, 0,     0,      0,
    0,    1.0f,  0,      0,
    0,    0,    -1.0f, -1.0f,
    0,    0,    -p0d40,  0.0f
};
#endif
#endif