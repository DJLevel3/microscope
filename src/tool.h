#pragma once

#include "config.h"
#include "system.h"

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>

void vec2ToSample(float* vec2, short* buffer);
void lineToSamples(float* vec3Start, float* vec3End, short* buffer, int samples);
bool strokeToCycle3D(float* points, int nPoints, short* buffer, int samples, float* viewMatrix);
bool strokeToCycle2D(float* points, int nPoints, short* buffer, int samples);
void addSamples(short* buffer, short* toAdd, int samples, float stretch);
//float scaleBuffer(short* buffer, int samples, float volume);
void fadeBuffer(short* buffer, int samples, float start, float end);
//static float vec3Distance(float* vec3A, float* vec3B);
void wobbleBufferEnv(short* buffer, int samples, float periodT, int phase, float scaleX, float scaleY, float intensityX, float intensityY, float curve);
//void wobbleBuffer(short* buffer, int samples, float periodT, int phase, float intensityX, float intensityY);
void filterBuffer(short* buffer, int samples, float resonance, float start, float end);
//void circleFill(short* buffer, int samples, int period, float level);
float squareWaveNoDC(int t, int period, float pulseWidth);
float squareWaveCentered(int t, int period, float pulseWidth);
float mn2f(float n);
void rotX(float* src, float* dest, float theta);
void rotY(float* src, float* dest, float theta);
void rotZ(float* src, float* dest, float theta);
void ballify(short* buffer, int samples, float radius, bool clip);
void addShade(short* buffer, int samples, int period, float amtX, float amtY);
void offsetBuffer(short* buffer, int samples, int offX, int offY);
void offsetBufferCheckered(short* buffer, int samples, int offX, int offY, int period2, int theta, float width);
void rotateBuffer(short* buffer, int samples, float theta);
void punchBuffer(short* buffer, int samples, int posX, int posY, float radius);
void gridBuffer(short* buffer, int samples, float edgeVal, int divisions);
void genDPCM(short* buffer, int samples, unsigned char* sample, int sampleBytes, float volume);
void shuffleBuffer(short* buffer, int samples, int* seed);

// gen should return in the range {0, 255}
bool hilligoss(short* buffer, int samples, int randomSeed, float tInitial, float tRate, unsigned char(*gen)(short x, short y, float t), int stretch);
bool determinePath(short* pixelsOriginal, int targetCount);

#define MAX_HILLIGOSS_ITERATIONS (1000 * 1000 * 10)


