//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2021                                   //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <GL/gl.h>
#include <math.h>
#include "config.h"
#include "system.h"
#include "shader.inl"
#include "fp.h"

//=================================================================================================================

static int   fsid;

int intro_init( void )
{
    int vsid = oglCreateShaderProgramv( GL_VERTEX_SHADER,   1, &vsh );
        fsid = oglCreateShaderProgramv( GL_FRAGMENT_SHADER, 1, &fsh );
 
    unsigned int pid;
    oglGenProgramPipelines(1, &pid);
    oglBindProgramPipeline(pid);
    oglUseProgramStages(pid, GL_VERTEX_SHADER_BIT, vsid);
    oglUseProgramStages(pid, GL_FRAGMENT_SHADER_BIT, fsid);

    #ifdef _DEBUG
        int		result;
        char    info[1536];
        oglGetProgramiv( vsid, GL_LINK_STATUS, &result ); oglGetProgramInfoLog( vsid, 1024, NULL, (char *)info );  if( !result ) DebugBreak();
        oglGetProgramiv( fsid, GL_LINK_STATUS, &result ); oglGetProgramInfoLog( fsid, 1024, NULL, (char *)info ); if( !result ) DebugBreak();
        oglGetProgramiv( pid,  GL_LINK_STATUS, &result ); oglGetProgramInfoLog( pid,  1024, NULL, (char *)info ); if( !result ) DebugBreak();
    #endif

    return 1;
}

//=================================================================================================================


static unsigned int audioBuffer[4800];

void intro_do( long time, long deltaTime, float* audioData, int count )
{
    //--- update parameters -----------------------------------------

    const float t  = 0.001f*(float)time;
    const float dt = 0.001f*(float)deltaTime;
    float timeI;

    float x, y;

    for (int i = 0; i < 4800; i++) {
        timeI = max(0.f, (t + ((i-1600.f) / 192000.f) * deltaTime / (1000.f / 60.f)));
        if (timeI * 192000 < count) {
            x = audioData[2 * f2i(timeI * 192000)];
            y = audioData[2 * f2i(timeI * 192000) + 1];
            audioBuffer[i] = f2i(SHRT_MAX * (1 + x)) + (f2i(SHRT_MAX * (1 + y)) << 16);
        }
        else audioBuffer[i] = 0b10000000000000001000000000000000;
    }

    //--- render -----------------------------------------
    oglProgramUniform1uiv(fsid, 0, 4800, audioBuffer);

    glRects( -1, -1, 1, 1 );
}