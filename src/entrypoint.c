//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008/2021                                   //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>
#include "system.h"
#include "config.h"
#include "microscope.h"

#ifdef STANDALONE
#include "tinywav.h"
#else
#include "demo.h"
static const unsigned int dotWavHeader[11] = {
    0x46464952,
    SAMPLE_RATE * AUDIO_SECONDS * 2 * 2 + 36,
    0x45564157,
    0x20746D66,
    16,
    WAVE_FORMAT_PCM | (2 << 16),
    FILE_RATE,
    FILE_RATE * 2 * sizeof(short),
    (2 * sizeof(short)) | ((8 * sizeof(short)) << 16),
    0x61746164,
    SAMPLE_RATE * 2 * AUDIO_SECONDS * sizeof(short)
};
#endif

static const PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
    32, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 32, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

const static char* glFuncNames[] = {
    "glCreateShaderProgramv",
    "glGenProgramPipelines",
    "glBindProgramPipeline",
    "glUseProgramStages",
    "glProgramUniform1ui",
    "glActiveTexture",
#ifdef _DEBUG
    //--
    "glGetProgramiv",
    "glGetProgramInfoLog"
#endif
};


int  _fltused = 0;

HDC hDC;
HGLRC hGLRC;
#ifdef _DEBUG
void* myglfunc[8];
#else
void* myglfunc[6];
#endif

HWAVEOUT wave_out;
#define CHUNK_SIZE 8192
WAVEHDR header[2] = { 0 };
short chunks[2][CHUNK_SIZE * 2];
bool chunk_swap = false;
long fileCounter;
bool safe = true;
bool released[4] = { true, true, true, true };

void CALLBACK WaveOutProc(HWAVEOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
short* audioData = NULL;
int audioCounter = 0;
int audioCounterMax = 0;
bool audioDone = false;
int exitCounter = 0;

float* graticule = NULL;
float* otherGraticule = NULL;
float scale = 1;
bool fucked = false;

#ifndef STANDALONE
DWORD WINAPI DemoThread(LPVOID parameters)
{
    float DEMO_DURATION;
    int DEMO_NUMSAMPLESC;

    bool done = 0;
    long t = timeGetTime();
    long st = 0;
    // init and build the demo
    if (!demo_init(t)) return 1;
    int section = 0;

    short* demoBuffer;

    done = GetAsyncKeyState(VK_ESCAPE);
    long writePointer = 22;

    st = timeGetTime();
    while (section < DEMO_SECTIONS && !done) {
        DEMO_NUMSAMPLESC = f2i((demo_length(section)) * SAMPLE_RATE * 2);

        demoBuffer = (short*)malloc((SAMPLE_RATE * 2 + DEMO_NUMSAMPLESC) * sizeof(short));
        if (demoBuffer != 0) {
            t = timeGetTime();
            done = !demo_do(t, demoBuffer, section, false);
            memmv(audioData + writePointer, demoBuffer, (DEMO_NUMSAMPLESC) * sizeof(short));
            writePointer += DEMO_NUMSAMPLESC;
            free(demoBuffer);
        }
        section++;
        done = done || GetAsyncKeyState(VK_ESCAPE);
    }

    demo_end();

    FILE* oFile = fopen("demo.wav", "wb");
    if (oFile != NULL) {
        fwrite(audioData, sizeof(short), SAMPLE_RATE * 2 * AUDIO_SECONDS + 22, oFile);
        fclose(oFile);
    }

    printf("Demo audio written!\n");
    return 0;
}
#endif
bool grat = false;
bool invalid = false;
//----------------------------------------------------------------------------
#ifdef STANDALONE
char filename[256] = ".\\audio.wav";
char entry[256];
#endif
const float scales[] = {
    1.f, 1.3f, 1.5f, 2.f, 3.f, 5.f };
char* scaleNames[] = {
    "Native",
    "High",
    "Medium",
    "Low",
    "iGPU",
    "Potato"
};
const int nscales = 6;
void entrypoint()
{
    float speed = 1.0f;
    int rscale = 1;
    long long nSamples;
    printf("microscope\nby DJ_Level_3/BUS ERROR Collective^Teletype Corporation\n\n");
    printf("           _\n");
    printf(" _ __ ___ (_) ___ _ __ ___  ___  ___ ___  _ __   ___\n");
    printf("| '_ ` _ \\| |/ __| '__/ _ \\/ __|/ __/ _ \\| '_ \\ / _ \\\n");
    printf("| | | | | | | (__| | | (_) \\__ \\ (_| (_) | |_) |  __/ \n");
    printf("|_| |_| |_|_|\\___|_|  \\___/|___/\\___\\___/| .__/ \\___|\n");
#ifdef STANDALONE
    printf("                by DJ_Level_3/BEC^TTC    |_|  v1.0.0\n\n");
#else
    printf("                by DJ_Level_3/BEC^TTC    |_| v1.0.0-R\n\n");
#endif
    printf("Press 1 to toggle between 2x scale (default) and 1x scale.\n");
    printf("Press 2 to toggle graticule.\n");
    printf("Press 3 to cycle through performance modes.\n");
 /**
 *                _
 *      _ __ ___ (_) ___ _ __ ___  ___  ___ ___  _ __   ___
 *     | '_ ` _ \| |/ __| '__/ _ \/ __|/ __/ _ \| '_ \ / _ \
 *     | | | | | | | (__| | | (_) \__ \ (_| (_) | |_) |  __/
 *     |_| |_| |_|_|\___|_|  \___/|___/\___\___/| .__/ \___|
 *                     by DJ_Level_3/BEC^TTC    |_|   v1.0.0
 *                     by DJ_Level_3/BEC^TTC    |_|   v1.0.0
 */
#ifdef STANDALONE
    TinyWav audio;
    int rate;
    printf("\nEnter audio file name[default - .\\audio.wav]: ");
    scanf_s("%255[^\n]s", entry, 256);
    rate = tinywav_open_read(&audio, entry);
    if (!rate) rate = tinywav_open_read(&audio, filename);
    speed = rate / 192000.f;
    if (!rate) ExitProcess(-2);
    if (audio.numChannels != 2) {
        fclose(audio.f);
        ExitProcess(-3);
    }
    nSamples = audio.numFramesInHeader;
    audioData = (short*)malloc(nSamples * 2 * sizeof(short));
    if (audioData == NULL) ExitProcess(-5);
    if (tinywav_read_f(&audio, audioData, nSamples) != nSamples) {
        fclose(audio.f);
        free(audioData);
        ExitProcess(-4);
    }
    fclose(audio.f);
#else
    audioData = (short*)malloc((FILE_RATE * 2 * AUDIO_SECONDS * 2) * sizeof(short));
    if (audioData == 0) ExitProcess(-420);
    memcl(audioData, 0, FILE_RATE * AUDIO_SECONDS * 2 + 44);
    memmv(audioData, dotWavHeader, 44);
    DWORD id = 0;
    HANDLE hDemo = CreateThread(NULL, 0, DemoThread, NULL, 0, &id);
    if (!hDemo) ExitProcess(-123);
    printf("\nBuffering while demo does initial calculations...\n\n");
    Sleep(1000);
    printf("Starting microscope!\n");
    printf("Performance mode %d - %s (%3.1fx scaling)\n", rscale, scaleNames[rscale], scales[rscale]);
    nSamples = FILE_RATE * AUDIO_SECONDS;
#endif

    audioCounterMax = nSamples;

    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    ShowCursor(0);

    // create window
    HWND hWnd = CreateWindow((LPCSTR)0xC018, 0, WS_VISIBLE, 0, 0, 540, 540, 0, 0, 0, 0);
    DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    DWORD dwExStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
    HMENU menu = GetMenu(hWnd);
    RECT rc = { 0,0,XRES, YRES };
    AdjustWindowRectEx(&rc, dwStyle, menu ? TRUE : FALSE, dwExStyle);
    SetWindowPos(hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOMOVE);

    if (!hWnd) return;
    hDC = GetDC(hWnd);
    // initalize opengl
    if (!SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd)) return;
    wglMakeCurrent(hDC, wglCreateContext(hDC));
    ((BOOL(WINAPI*)(int))wglGetProcAddress("wglSwapIntervalEXT"))(1);

#ifdef _DEBUG
    for (int i = 0; i < 8; i++)
#else
    for (int i = 0; i < 6; i++)
#endif
    {
        myglfunc[i] = wglGetProcAddress(glFuncNames[i]);
        if (!myglfunc[i])
            ExitProcess(i);
    }

    graticule = generateGraticule(true);
    if (graticule == NULL) {
        free(audioData);
        ExitProcess(16);
    }

    otherGraticule = generateGraticule(false);
    if (otherGraticule == NULL) {
        free(audioData);
        free(graticule);
        ExitProcess(17);
    }

    // init intro
    if (!microscope_init()) return;

    WAVEFORMATEX format;
    format.wFormatTag = WAVE_FORMAT_PCM,
    format.nChannels = 2;
    format.wBitsPerSample = 16;
    format.cbSize = 0;
    format.nBlockAlign = 4;
#ifdef STANDALONE
    format.nSamplesPerSec = rate;
    format.nAvgBytesPerSec = rate*4;
#else
    format.nSamplesPerSec = FILE_RATE;
    format.nAvgBytesPerSec = FILE_RATE * 4;
#endif

    if (waveOutOpen(&wave_out, -1, &format, (DWORD_PTR)WaveOutProc, (DWORD_PTR)NULL, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
        free(audioData);
        free(graticule);
        free(otherGraticule);
        ExitProcess(-1);
    }

    for (int i = 0; i < 2; ++i) {
        memcl(chunks[i], 0, 2 * CHUNK_SIZE * sizeof(short));
        header[i].lpData = (CHAR*)chunks[i];
        header[i].dwBufferLength = CHUNK_SIZE * 2 * 2;
        if (waveOutPrepareHeader(wave_out, &header[i], sizeof(header[i])) != MMSYSERR_NOERROR) {
            free(audioData);
            free(graticule);
            free(otherGraticule);
            ExitProcess(-2);
        }
        if (waveOutWrite(wave_out, &header[i], sizeof(header[i])) != MMSYSERR_NOERROR) {
            free(audioData);
            free(graticule);
            free(otherGraticule);
            ExitProcess(-3);
        }
    }

    // play intro
    long t;
    long tZero = timeGetTime();
    safe = true;
    MSG msg;
    do
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (GetAsyncKeyState('1')) {
            if (released[0]) scale = 1 - scale;
            released[0] = false;
        }
        else released[0] = true;

        if (GetAsyncKeyState('2')) {
            if (released[1]) {
                grat = !grat;
            }
            released[1] = false;
        }
        else released[1] = true;

        if (GetAsyncKeyState('3')) {
            if (released[2]) {
                rscale = (rscale + 1) % nscales;
                printf("Performance mode %d - %s (%3.1fx scaling)\n", rscale, scaleNames[rscale], scales[rscale]);
            }
            released[2] = false;
        }
        else released[2] = true;

        t = timeGetTime();
        if (!fucked) microscope_do(audioCounter, audioData + 44, audioCounterMax, speed, scale + 1.0f, grat ? otherGraticule : graticule, scales[rscale]);

        wglSwapLayerBuffers(hDC, WGL_SWAP_MAIN_PLANE);
        if (audioDone) exitCounter++;
    } while (!fucked && msg.message != WM_QUIT && !GetAsyncKeyState(VK_ESCAPE) && exitCounter < 10);

    ShowCursor(1);

    fucked = true;
    safe = false;
    
    // free up audio or whatever
#ifndef STANDALONE
    WaitForSingleObject(hDemo, INFINITE);
    CloseHandle(hDemo);
#endif

    free(audioData);
    free(graticule);
    free(otherGraticule);

    ExitProcess(0);
}

void CALLBACK WaveOutProc(HWAVEOUT wave_out_handle, UINT message, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2) {
    if (message == WOM_DONE && !invalid) {
        if (safe) {
            for (int i = 0; i < CHUNK_SIZE; ++i) {
                // write the audio here
#ifdef STANDALONE
                chunks[chunk_swap][i * 2] = audioData[audioCounter * 2];
                chunks[chunk_swap][i * 2 + 1] = audioData[audioCounter * 2 + 1];
#else
                chunks[chunk_swap][i * 2] = audioData[22 + audioCounter * 2];
                chunks[chunk_swap][i * 2 + 1] = audioData[22 + audioCounter * 2 + 1];
#endif
                if (audioCounter + 1 < audioCounterMax) audioCounter++;
                else audioDone = true;
            }
        }
        waveOutWrite(wave_out_handle, &header[chunk_swap], sizeof(header[chunk_swap]));
        chunk_swap = !chunk_swap;
    }
    else if (message == WOM_CLOSE) {
        invalid = true;
        fucked = true;
        safe = false;
    }
}
