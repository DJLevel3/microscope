//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2021                                   //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>
#include "system.h"
#include "config.h"
#include "intro.h"
#include "tinywav.h"

static const PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
    32, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 32, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

static DEVMODE screenSettings = { {0},
    #if _MSC_VER < 1400
    0,0,148,0,0x001c0000,{0},0,0,0,0,0,0,0,0,0,{0},0,32,XRES,YRES,0,0,      // Visual C++ 6.0
    #else
    0,0,156,0,0x001c0000,{0},0,0,0,0,0,{0},0,32,XRES,YRES,{0}, 0,           // Visual Studio 2005
    #endif
    #if(WINVER >= 0x0400)
    0,0,0,0,0,0,
    #if (WINVER >= 0x0500) || (_WIN32_WINNT >= 0x0400)
    0,0
    #endif
    #endif
};

static char* glFuncNames[] = {
    "glCreateShaderProgramv",
    "glGenProgramPipelines",
    "glBindProgramPipeline",
    "glUseProgramStages",
    "glProgramUniform1uiv",
#ifdef _DEBUG
    //--
    "glGetProgramiv",
    "glGetProgramInfoLog"
#endif
};


int  _fltused = 0;

HDC hDC;
HGLRC hGLRC;
void* myglfunc[5];

HWAVEOUT wave_out;
#define CHUNK_SIZE 8192
WAVEHDR header[2] = { 0 };
short chunks[2][CHUNK_SIZE * 2];
bool chunk_swap = false;
long fileCounter;
bool safe = true;

void CALLBACK WaveOutProc(HWAVEOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
float* audioData = NULL;
int audioCounter = 0;
int audioCounterMax = 0;
bool audioDone = false;
int exitCounter = 0;

//----------------------------------------------------------------------------

void entrypoint()
{
    // For graphics
    TinyWav audio;
    if (tinywav_open_read(&audio, ".\\audio.wav", TW_INTERLEAVED)) ExitProcess(-2);
    if (audio.numChannels != 2) {
        tinywav_close_read(&audio);
        ExitProcess(-3);
    }
    int nSamples = audio.numFramesInHeader;
    audioData = (float*)malloc(nSamples * 2 * sizeof(float));
    if (audioData == NULL) ExitProcess(-5);
    if (tinywav_read_f(&audio, audioData, nSamples) != nSamples) {
        tinywav_close_read(&audio);
        free(audioData);
        ExitProcess(-4);
    }
    tinywav_close_read(&audio);

    audioCounterMax = nSamples;

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    ShowCursor(0);

    // create window
    //HWND hWnd = CreateWindow( "static",0,WS_POPUP|WS_VISIBLE,0,0,XRES,YRES,0,0,0,0);
    HWND hWnd = CreateWindow((LPCSTR)0xC018, 0, WS_POPUP | WS_VISIBLE, 0, 0, XRES, YRES, 0, 0, 0, 0);
    if (!hWnd) return;
    hDC = GetDC(hWnd);
    // initalize opengl
    if (!SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd)) return;
    wglMakeCurrent(hDC, wglCreateContext(hDC));

#ifdef _DEBUG
    for (int i = 0; i < 7; i++)
#else
    for (int i = 0; i < 5; i++)
#endif
    {
        myglfunc[i] = wglGetProcAddress(glFuncNames[i]);
        if (!myglfunc[i])
            return;
    }

    // init intro
    if (!intro_init()) return;

    WAVEFORMATEX format;
    format.wFormatTag = WAVE_FORMAT_PCM,
    format.nChannels = 2;
    format.nSamplesPerSec = 192000;
    format.wBitsPerSample = 16;
    format.cbSize = 0;
    format.nBlockAlign = 4;
    format.nAvgBytesPerSec = 768000;

    if (waveOutOpen(&wave_out, -1, &format, (DWORD_PTR)WaveOutProc, (DWORD_PTR)NULL, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        free(audioData);
        ExitProcess(-1);
    }

    for (int i = 0; i < 2; ++i) {
        memset(chunks[i], 0, 2 * CHUNK_SIZE * sizeof(short));
        header[i].lpData = (CHAR*)chunks[i];
        header[i].dwBufferLength = CHUNK_SIZE * 2 * 2;
        if (waveOutPrepareHeader(wave_out, &header[i], sizeof(header[i])) != MMSYSERR_NOERROR) {
            free(audioData);
            ExitProcess(-2);
        }
        if (waveOutWrite(wave_out, &header[i], sizeof(header[i])) != MMSYSERR_NOERROR) {
            free(audioData);
            ExitProcess(-3);
        }
    }

    // play intro
    long t;
    long tZero = timeGetTime();
    long lastT = tZero - 10;
    do
    {
        t = timeGetTime();
        intro_do(t - tZero, t - lastT, audioData, audioCounterMax);
        lastT = t;
        wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE); //SwapBuffers( hDC );
        if (audioDone) exitCounter++;
    } while (!GetAsyncKeyState(VK_ESCAPE) && exitCounter < 10);

    ShowCursor(1);

    waveOutUnprepareHeader(wave_out, &header[0], sizeof(header[0]));
    waveOutUnprepareHeader(wave_out, &header[0], sizeof(header[1]));

    safe = false;
    
    // free up audio or whatever
    free(audioData);

    ExitProcess(0);
}

void CALLBACK WaveOutProc(HWAVEOUT wave_out_handle, UINT message, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2) {
    if (message == WOM_DONE) {
        if (safe) {
            for (int i = 0; i < CHUNK_SIZE; ++i) {
                // write the audio here
                chunks[chunk_swap][i * 2] = f2i(SHRT_MAX * audioData[audioCounter * 2]);
                chunks[chunk_swap][i * 2 + 1] = f2i(SHRT_MAX * audioData[audioCounter * 2 + 1]);
                if (audioCounter + 1 < audioCounterMax) audioCounter++;
                else audioDone = true;
            }
        }
        if (waveOutWrite(wave_out, &header[chunk_swap], sizeof(header[chunk_swap])) != MMSYSERR_NOERROR) {
            // something's gone wrong, idk
        }
        chunk_swap = !chunk_swap;
    }
}