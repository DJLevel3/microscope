//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2021                                   //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include "intro.h"

//=================================================================================================================

static int   fsid;
static int   stex[2];

int intro_init( float* graticule )
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

    glGenTextures(2, stex);
    glBindTexture(GL_TEXTURE_2D, stex[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 300, 300, 0, GL_RGB, GL_FLOAT, graticule);

    oglActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, stex[0]);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return 1;
}

//=================================================================================================================

static float audioBuffer[2*4800];

void intro_do( long time, long deltaTime, float* audioData, int count, float speed, float scale )
{
    //--- update parameters -----------------------------------------
    float t = (speed * time * 192), dt = speed * deltaTime * 0.06f;
    int timeI;
    memcl(audioBuffer, 2 * 4800 * sizeof(float));
    for (int i = 0; i < 4800; i++) {
        timeI = f2i(max(0.f, (t + (i-1600) * dt)));
        if (timeI < count) {
            audioBuffer[i * 2] = 0.5f * scale * audioData[2 * timeI];
            audioBuffer[i * 2 + 1] = 0.5f * scale * audioData[2 * timeI + 1];
        }
        else {
            break;
        }
    }

    //--- render -----------------------------------------
    //oglProgramUniform1uiv(fsid, 0, 4080, audioBuffer);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RG32F, 4800, 0, GL_RG, GL_FLOAT, audioBuffer);

    glRects( -1, -1, 1, 1 );
}

// make sure to free the returned array!
float* generateGraticule() {
    float* g = (float*)malloc(3 * 300 * 300 * sizeof(float));
    memcl(g, 3 * 300 * 300 * sizeof(float));
    if (g == NULL) return NULL;
    for (int x = 1; x < 301; x++) {
        for (int y = 1; y < 301; y++) {
            if (!(x % 60 < 2
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