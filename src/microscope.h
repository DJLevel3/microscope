//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2021                                   //
//--------------------------------------------------------------------------//

#ifndef _INTRO_H_
#define _INTRO_H_

#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#include "config.h"
#include "system.h"
#include "shader.inl"
#include "fp.h"
#include <stdio.h>

int  microscope_init( );

void microscope_do( long time, short* audioData, int count, float speed, float scale, float* graticule, float render_scale );

float* generateGraticule(bool on);

#endif
