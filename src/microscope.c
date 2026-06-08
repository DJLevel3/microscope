//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2021                                   //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include "microscope.h"

//=================================================================================================================

static int   fsid;
static int   stex[3];

int microscope_init( )
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
    oglGetProgramiv(vsid, GL_LINK_STATUS, &result); oglGetProgramInfoLog(vsid, 1024, NULL, (char*)info);  if (!result) {
        printf("Vertex shader compilation error log:\n%s\n\n", info);
    }
    oglGetProgramiv(fsid, GL_LINK_STATUS, &result); oglGetProgramInfoLog(fsid, 1024, NULL, (char*)info); if (!result) {
        printf("Fragment shader compilation error log:\n%s\n\n", info);
    }
    oglGetProgramiv( pid,  GL_LINK_STATUS, &result ); oglGetProgramInfoLog( pid,  1024, NULL, (char *)info ); if( !result ) {
        printf("Shader creation error log : \n % s\n\n", info);
    }
    #endif

    glGenTextures(3, stex);
    oglActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, stex[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    oglActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, stex[0]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    oglActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, stex[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return 1;
}

//=================================================================================================================

static float audioBuffer[2*9601];

void microscope_do( long time, short* audioData, int count, float speed, float scale, float* graticule, float render_scale)
{
    //--- update parameters -----------------------------------------
    int timeI;
    memcl(audioBuffer, 0, 2 * 9601 * sizeof(float));
    int t = f2i(time * speed);
    audioBuffer[1] = render_scale;
    for (int i = 1; i < 9601; i++) {
        timeI = (t + f2i((i-3201) * speed));
        if (timeI < 0 || timeI >= count) {
            audioBuffer[i * 2] = 0;
            audioBuffer[i * 2 + 1] = 0;
        } else {
            audioBuffer[i * 2] = 0.5f * scale * audioData[2 * timeI] / 32768;
            audioBuffer[i * 2 + 1] = 0.5f * scale * audioData[2 * timeI + 1] / 32768;
        }
    }
    
    oglActiveTexture(GL_TEXTURE0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 300, 300, 0, GL_RGB, GL_FLOAT, graticule);

    oglActiveTexture(GL_TEXTURE1);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RG32F, 9601, 0, GL_RG, GL_FLOAT, audioBuffer);
    glRectf(-1.f/ render_scale, -1.f/ render_scale, 1.f/ render_scale, 1.f/ render_scale);

    audioBuffer[0] = 1.f;
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RG32F, 1, 0, GL_RG, GL_FLOAT, audioBuffer);

    oglActiveTexture(GL_TEXTURE2);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 0, 0, XRES, YRES, 0);
    glRects(-1, -1, 1, 1);
}

// make sure to free the returned array!
float* generateGraticule(bool on) {
    float* g = (float*)malloc(3 * 300 * 300 * sizeof(float));
    memcl(g, 0, 3 * 300 * 300 * sizeof(float));
    if (g == NULL) return NULL;
    for (int x = 1; x < 301; x++) {
        for (int y = 1; y < 301; y++) {
            if (!on || !(x % 60 < 2
                || y % 60 < 2
                || (x < 6 && y % 12 < 2)
                || (y < 6 && x % 12 < 2)
                || (y % 150 < 2 && ((x + 2) % 12 < 6)))) {
                g[3 * ((x - 1) + 300 * (y - 1))] = 1.0f;
            }
        }
    }
    return g;
}