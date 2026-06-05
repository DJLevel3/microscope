#include "tool.h"

/*
static float vec3Distance(float* vec3A, float* vec3B)
{
    float dist = 0;
    float diff[3] = { vec3B[0] - vec3A[0],
                      vec3B[1] - vec3A[1],
                      vec3B[2] - vec3A[2] };

    dist += diff[0] * diff[0];
    dist += diff[1] * diff[1];
    dist += diff[2] * diff[2];

    return sqrtf(dist);
}*/

static float vec2Distance(float* vec2A, float* vec2B)
{
    float diff[2] = { vec2B[0] - vec2A[0],
                      vec2B[1] - vec2A[1] };

    return sqrtf(diff[0] * diff[0] + diff[1] * diff[1]);
}

void vec2ToSample(float* vec2, short* buffer)
{
    buffer[0] = f2i(vec2[0] * SHRT_MAX);
    buffer[1] = f2i(vec2[1] * SHRT_MAX);
}

void lineToSamples(float* vec3Start, float* vec3End, short* buffer, int samples)
{
    float pos[2];
    float dPos[2];
    float delta = 1.0f / samples;

    pos[0] = vec3Start[0];
    pos[1] = vec3Start[1];

    dPos[0] = (vec3End[0] - pos[0]) * delta;
    dPos[1] = (vec3End[1] - pos[1]) * delta;
    for (int i = 0; i < samples; i++) {
        vec2ToSample(pos, buffer + (2 * i));
        pos[0] += dPos[0];
        pos[1] += dPos[1];
    }
}

bool strokeToCycle3D(float* points, int nPoints, short* buffer, int samples, float* viewMatrix)
{
    if (nPoints < 2) return false;
    float distance = 0;

    // allocate floats for lengths
    float* lengths = (float*)malloc(nPoints * sizeof(float));
    if (lengths == 0) return false;

    for (int i = 0; i < nPoints; i++) {
        mvp43(viewMatrix, points + i * 3, points + i * 3);
        mvp43(proj_matrix, points + i * 3, points + i * 3);
    }

    // calculate line lengths and total frame distance
    for (int i = 0; i < nPoints - 1; i++) {
        lengths[i] = vec2Distance(points + i * 3, points + ((i + 1) * 3));
        distance += lengths[i];
    }
    if (distance <= 0) return false;

    float lengthFactor = samples / distance;

    int bCounter = 0;
    int dS;

    for (int i = 0; i < nPoints - 1; i++) {
        dS = f2i(lengths[i] * lengthFactor);
        lineToSamples(points + i * 3, points + ((i + 1) * 3), buffer + (2 * bCounter), dS);
        bCounter += dS;
    }
    while (bCounter < samples) {
        buffer[bCounter * 2] = buffer[bCounter * 2 - 2];
        buffer[bCounter * 2 + 1] = buffer[bCounter * 2 - 1];
        bCounter++;
    }

    // deallocate scratch
    free(lengths);

    return true;
}

bool strokeToCycle2D(float* points, int nPoints, short* buffer, int samples)
{
    if (nPoints < 2) return false;
    float distance = 0;

    // allocate floats for lengths
    float* lengths = (float*)malloc(nPoints * sizeof(float));
    if (lengths == 0) return false;

    // calculate line lengths and total frame distance
    for (int i = 0; i < nPoints - 1; i++) {
        lengths[i] = vec2Distance(points + i * 3, points + ((i + 1) * 3));
        distance += lengths[i];
    }
    if (distance <= 0) return false;

    float lengthFactor = samples / distance;

    int bCounter = 0;
    int dS;

    for (int i = 0; i < nPoints - 1; i++) {
        dS = f2i(lengths[i] * lengthFactor);
        lineToSamples(points + i * 3, points + ((i + 1) * 3), buffer + (2 * bCounter), dS);
        bCounter += dS;
    }
    while (bCounter < samples) {
        buffer[bCounter * 2] = buffer[bCounter * 2 - 2];
        buffer[bCounter * 2 + 1] = buffer[bCounter * 2 - 1];
        bCounter++;
    }

    // deallocate scratch
    free(lengths);
    
    return true;
}

void addSamples(short* buffer, short* toAdd, int samples, float stretch)
{
    int l, r;
    for (int i = 0; i < f2i(samples * stretch); i++) {
        l = buffer[i * 2] + toAdd[f2i(i / stretch) * 2];
        l = (l > SHRT_MAX ? SHRT_MAX : l);
        l = (l < SHRT_MIN ? SHRT_MIN : l);

        r = buffer[i * 2 + 1] + toAdd[f2i(i / stretch) * 2 + 1];
        r = (r > SHRT_MAX ? SHRT_MAX : r);
        r = (r < SHRT_MIN ? SHRT_MIN : r);

        buffer[i * 2] = l;
        buffer[i * 2 + 1] = r;

        if (!(i & 0b1111) && GetAsyncKeyState(VK_ESCAPE)) return;
    }
}

/*
float scaleBuffer(short* buffer, int samples, float volume)
{
    for (int i = 0; i < samples * 2; i++) {
        buffer[i] = max(SHRT_MIN, min(SHRT_MAX, f2i(buffer[i] * volume)));

        if (!(i & 0b10000) && GetAsyncKeyState(VK_ESCAPE)) return 0;
    }

    return volume;
}*/

void fadeBuffer(short* buffer, int samples, float start, float end)
{
    float delta = (end - start) / samples;
    for (int i = 0; i < samples; i++) {
        buffer[i * 2] = f2i(buffer[i * 2] * (start + i * delta));
        buffer[i * 2 + 1] = f2i(buffer[i * 2 + 1] * (start + i * delta));

        if (!(i & 0b1111) && GetAsyncKeyState(VK_ESCAPE)) return;
    }
}

void wobbleBufferEnv(short* buffer, int samples, float periodT, int phase, float scaleX, float scaleY, float intensityX, float intensityY, float curve)
{
    float x, y, wx, wy, ex, ey;
    for (int i = 0; i < samples; i++) {
        x = buffer[i * 2] / (float)SHRT_MAX;
        y = buffer[i * 2 + 1] / (float)SHRT_MAX;

        wx = sinf(((i + phase) / periodT + scaleX * y) * 2.f * M_PI_F);
        wy = sinf(((i + phase) / periodT + scaleY * x) * 2.f * M_PI_F);

        ex = (powf((float)(M_E), -curve * x * x) - powf((float)(M_E), -curve)) / (1 - powf((float)(M_E), -curve));
        ey = (powf((float)(M_E), -curve * y * y) - powf((float)(M_E), -curve)) / (1 - powf((float)(M_E), -curve));

        buffer[i * 2] = min(SHRT_MAX, max(SHRT_MIN, buffer[i * 2] + f2i(ex * wx * intensityX * SHRT_MAX)));
        buffer[i * 2 + 1] = min(SHRT_MAX, max(SHRT_MIN, buffer[i * 2 + 1] + f2i(ey * wy * intensityY * SHRT_MAX)));

        if (!(i & 0b1111) && GetAsyncKeyState(VK_ESCAPE)) {
            return;
        }
    }
}
/*
void wobbleBuffer(short* buffer, int samples, float periodT, int phase, float intensityX, float intensityY)
{
    float x, y, wx, wy;
    for (int i = 0; i < samples; i++) {
        x = buffer[i * 2] / (float)SHRT_MAX;
        y = buffer[i * 2 + 1] / (float)SHRT_MAX;

        wx = sinf(((i + phase) / periodT) * 2.f * M_PI);
        wy = sinf(((i + phase) / periodT) * 2.f * M_PI);

        buffer[i * 2] = min(SHRT_MAX, max(SHRT_MIN, buffer[i * 2] + f2i(wx * intensityX * SHRT_MAX)));
        buffer[i * 2 + 1] = min(SHRT_MAX, max(SHRT_MIN, buffer[i * 2 + 1] + f2i(wy * intensityY * SHRT_MAX)));
    }
}*/
/*
void circleFill(short* buffer, int samples, int period, float level)
{
    for (int i = 0; i < samples; i++) {
        buffer[i * 2] = f2i(SHRT_MAX * sin(i * 2.f * M_PI / period) * level);
        buffer[i * 2 + 1] = f2i(SHRT_MAX * cos(i * 2.f * M_PI / period) * level);
    }
}*/
float squareWaveNoDC(int t, int period, float pulseWidth) {
    return ((t % period) <= pulseWidth * period) ? (pulseWidth - 1) : (pulseWidth);
}
float squareWaveCentered(int t, int period, float pulseWidth) {
    return ((t % period) <= pulseWidth * period) ? -0.5 : 0.5;
}

float mn2f(float n) {
    return SPEED_FACTOR * 261.63f * powf(2.0f, (n - 60.f) / 12.f);
}

void filterBuffer(short* buffer, int samples, float resonance, float start, float end)
{
    float state0_L = 0;
    float state1_L = 0;
    float state2_L = 0;

    float state0_R = 0;
    float state1_R = 0;
    float state2_R = 0;

    float fb, cut;
    float t = start;
    float dt = (end - start) / samples;
    for (int i = 0; i < samples; i++)
    {
        cut = max(0.00001, min(t, 0.999f));
        fb = min(0.9f, resonance + resonance / (1.0 - cut));

        state0_L += cut * (buffer[2 * i] - state0_L + fb * (state1_L - state2_L));
        state1_L += cut * (state0_L - state1_L);
        state2_L += cut * (state1_L - state2_L);
        buffer[2 * i] = f2i(state2_L);


        state0_R += cut * (buffer[2 * i + 1] - state0_R + fb * (state1_R - state2_R));
        state1_R += cut * (state0_R - state1_R);
        state2_R += cut * (state1_R - state2_R);
        buffer[2 * i + 1] = f2i(state2_R);
        t += dt;
    }
}

void rotX(float* src, float* dest, float theta) {
    float sT = sinf(theta);
    float cT = cosf(theta);

    float x, y;
    x = cT * src[1] - sT * src[2];
    y = sT * src[1] + cT * src[2];

    dest[0] = src[0];
    dest[1] = x;
    dest[2] = y;
}

void rotY(float* src, float* dest, float theta) {
    float sT = sinf(theta);
    float cT = cosf(theta);

    float x, y;
    x = cT * src[0] - sT * src[2];
    y = sT * src[0] + cT * src[2];

    dest[0] = x;
    dest[1] = src[1];
    dest[2] = y;
}

void rotZ(float* src, float* dest, float theta) {
    float sT = sinf(theta);
    float cT = cosf(theta);

    float x, y;

    x = cT * src[0] - sT * src[1];
    y = sT * src[0] + cT * src[1];

    dest[0] = x;
    dest[1] = y;
    dest[2] = src[2];
}


//#define HANDLE_SPECIAL
void ballify(short* buffer, int samples, float radius, bool clip)
{
    float r, t, x, y;
    for (int i = 0; i < samples; i++) {
        x = buffer[2 * i];
        y = buffer[2 * i + 1];
#ifdef HANDLE_SPECIAL
        // special cases
             if (x == 0 && y < 0) t = -M_PI_F / 2;
        else if (y == 0 && x >= 0) t = 0;
        else if (x == 0 && y >  0) t = M_PI_F / 2;
        else if (y == 0 && x <  0) t = M_PI_F;
        // if everything is positive we're good
        else
#endif
            t = atan2f(y, x);
        r = sqrtf(x * x + y * y) / SHRT_MAX / radius;
        if (clip) r = max(-1.f, min(r, 1.f));
        r = sinf(r * M_PI_F / 2) * SHRT_MAX * radius;
        buffer[2 * i] = f2i(r * cosf(t));
        buffer[2 * i + 1] = f2i(r * sinf(t));
    }
}
void addShade(short* buffer, int samples, int period, float amtX, float amtY) {
    int x, y;

    for (int i = 0; i < samples; i++) {
        x = f2i(buffer[i * 2] + amtX * SHRT_MAX * sinf(i * 2 * M_PI_F / period));
        y = f2i(buffer[i * 2 + 1] + amtY * SHRT_MAX * sinf(i * 2 * M_PI_F / period));

        buffer[2 * i] = max(SHRT_MIN, min(x, SHRT_MAX));
        buffer[2 * i + 1] = max(SHRT_MIN, min(y, SHRT_MAX));
    }
}
void offsetBuffer(short* buffer, int samples, int offX, int offY)
{
    for (int i = 0; i < samples; i++) {
        buffer[i * 2] += offX;
        buffer[i * 2 + 1] += offY;
    }
}
void offsetBufferCheckered(short* buffer, int samples, int offX, int offY, int period2, int theta, float width)
{
    float t;
    for (int i = 0; i < samples; i++) {
        t = squareWaveCentered(i + theta, period2, width);
        buffer[i * 2] += f2i(t * offX);
        buffer[i * 2 + 1] += f2i(t * offY);
    }
}
void rotateBuffer(short* buffer, int samples, float theta)
{
    float sT = sinf(theta);
    float cT = cosf(theta);

    float x, y;

    for (int i = 0; i < samples; i++) {
        x = cT * buffer[i * 2] - sT * buffer[i * 2 + 1];
        y = sT * buffer[i * 2] + cT * buffer[i * 2 + 1];
        buffer[i * 2] = f2i(x);
        buffer[i * 2 + 1] = f2i(y);
    }
}
void punchBuffer(short* buffer, int samples, int posX, int posY, float radius)
{
    float x, y, x2, y2, r, t;

    for (int i = 0; i < samples; i++) {
        x = buffer[2 * i];
        y = buffer[2 * i + 1];
        x2 = (x - posX) / SHRT_MAX;
        y2 = (y - posY) / SHRT_MAX;
        r = sqrtf(x2 * x2 + y2 * y2);

        if (r < radius) {
#ifdef HANDLE_SPECIAL
            // special cases
            if (x2 == 0 && y2 < 0) t = -M_PI_F / 2;
            else if (y2 == 0 && x2 >= 0) t = 0;
            else if (x2 == 0 && y2 > 0) t = M_PI_F / 2;
            else if (y2 == 0 && x2 < 0) t = M_PI_F;
            // if everything is positive we're good
            else
#endif
                t = atan2f(y2, x2);
            r = radius;
            buffer[2 * i] = posX + f2i(SHRT_MAX * r * cosf(t));
            buffer[2 * i + 1] = posY + f2i(SHRT_MAX * r * sinf(t));
        }

    }
}
void gridBuffer(short* buffer, int samples, float edgeVal, int divisions)
{
    int lines = (divisions + 2);
    float samplesPerLine = (float)(samples) / lines / 2;
    int s = 0;
    int nS = 0;
    float targ = samplesPerLine;
    float points[6] = { 0,-1,0,0,1,0 };
    for (int l = 0; l < lines; l++) {
        nS = f2i(targ - s);
        targ += samplesPerLine;

        points[0] = (l * 2.f / (lines - 1)) - 1.f;
        points[1] = points[1] * (1 - (l % 2) * 2);
        points[3] = points[0];
        points[4] = points[4] * (1 - (l % 2) * 2);
        lineToSamples(points, points + 3, buffer + 2 * s, nS);
        s += nS;
    }
    points[0] = -1.f;
    points[3] = 1.f;
    for (int l = 0; l < lines; l++) {
        nS = f2i(targ - s);
        targ += samplesPerLine;

        points[0] = points[0] * (1 - (l % 2) * 2);
        points[1] = (l * 2.f / (lines - 1)) - 1.f;
        points[3] = points[3] * (1 - (l % 2) * 2);
        points[4] = points[1];
        lineToSamples(points, points + 3, buffer + 2 * s, nS);
        s += nS;
    }
    fadeBuffer(buffer, samples, edgeVal, edgeVal);
}

bool hilligoss(short* buffer, int samples, int randomSeed, float tInitial, float tRate, unsigned char(*gen)(short x, short y, float t), int stretch) {
    short* plasmaPointsOrdered = (short*)malloc(samples * 2 * sizeof(short));
    if (plasmaPointsOrdered == 0) {
        memcl(buffer, 0, samples * 2 * sizeof(short));
        return 0;
    }
    short* plasmaPointsUnordered = (short*)malloc(samples * 2 * sizeof(short));
    if (plasmaPointsUnordered == 0) {
        free(plasmaPointsOrdered);
        memcl(buffer, 0, samples * 2 * sizeof(short));
        return 0;
    }
    float* lookup = (float*)malloc(256 * sizeof(float));
    if (lookup == 0) {
        free(plasmaPointsOrdered);
        free(plasmaPointsUnordered);
        memcl(buffer, 0, samples * 2 * sizeof(short));
        return 0;
    }
    int seed = randomSeed;
    short random1;
    short random2;
    int nPoints = 0;
    int nPointsSaver = 0;
    float z;
    float thresh = 0;
    long iterationCounter = 0;

    // Lookup table
    for (int i = 0; i < 256; i++) {
        z = i / 256.f;
        lookup[i] = z * z;
    }

    // choosePixels
    nPoints = 0;
    while (nPoints < samples && !GetAsyncKeyState(VK_ESCAPE)) {
        random1 = (demo_rand(&seed));
        random2 = (demo_rand(&seed));
        thresh = fabsf(demo_rand(&seed) / 32768.f);
        z = lookup[(*gen)(random1, random2, tInitial + tRate * nPoints)];
        if (z - lookup[30] > thresh) {
            plasmaPointsUnordered[nPoints * 2] = random1;
            plasmaPointsUnordered[nPoints * 2 + 1] = random2;
            nPoints++;
        }
        iterationCounter++;
        if (iterationCounter > MAX_HILLIGOSS_ITERATIONS) break;
    }

    if (nPoints == 0 || GetAsyncKeyState(VK_ESCAPE)) {
        free(lookup);
        free(plasmaPointsOrdered);
        free(plasmaPointsUnordered);
        memcl(buffer, 0, samples * 2 * sizeof(short));
        return 0;
    }
    nPointsSaver = 0;
    while (nPoints < samples) {
        plasmaPointsUnordered[nPoints * 2]     = plasmaPointsUnordered[nPointsSaver * 2];
        plasmaPointsUnordered[nPoints * 2 + 1] = plasmaPointsUnordered[nPointsSaver * 2 + 1];
        nPoints++;
        nPointsSaver++;
    }

    // determinePath
    if (determinePath(plasmaPointsUnordered, samples)) {
        for (int i = 0; i < samples; i++) {
            buffer[i * 2] = plasmaPointsUnordered[(i / stretch) * stretch * 2];
            buffer[i * 2 + 1] = plasmaPointsUnordered[(i / stretch) * stretch * 2 + 1];
        }
    }
    else {
        free(lookup);
        free(plasmaPointsOrdered);
        free(plasmaPointsUnordered);
        return 0;
    }
    free(lookup);
    free(plasmaPointsOrdered);
    free(plasmaPointsUnordered);
    return nPoints;
}

// Find an order through which the pixels should be traversed and convert it into 16-bit PCM audio
bool determinePath(short* pixelsOriginal, int targetCount)
{
    short* path = (short*)malloc(targetCount * 2 * sizeof(short));
    if (path == 0) {
        return 0;
    }
    int pathLength = 0;
    int nPix = targetCount;

    // While we haven't hit the target count, and while there are still pixels on the map...
    while (pathLength < targetCount && nPix > 0)
    {
        if (GetAsyncKeyState(VK_ESCAPE)) {
            free(path);
            return 0;
        }
        // Add that starting point to the path
        path[pathLength * 2] = pixelsOriginal[0];
        path[pathLength * 2 + 1] = pixelsOriginal[1];
        pathLength++;
        nPix--;

        // Remove the starting point from the map
        pixelsOriginal[0] = pixelsOriginal[nPix * 2];
        pixelsOriginal[1] = pixelsOriginal[nPix * 2 + 1];

        for (int jumpCounter = 0; jumpCounter < 10000 && pathLength < targetCount; jumpCounter++)
        {
            // Closest index is invalid for now
            long closestIndex = -1;

            // Closest distance is infinite for now
            float minDistance = 655360;

            for (int pixel = 0; pixel < nPix; pixel++) {
                short x = pixelsOriginal[pixel * 2];
                short y = pixelsOriginal[pixel * 2 + 1];
                // If the pixel is within search distance
                if (fabsf((float)(x) - path[pathLength * 2 - 2]) < 4096 && fabsf((float)(y) - path[pathLength * 2 - 1]) < 4096) {
                    // Calculate the distance to the current pixel (sqrt(dx^2 + dy^2))
                    float dist = sqrtf(powf((float)(path[(pathLength * 2 - 2)]) - (float)(x), 2.0) + powf((float)(path[(pathLength * 2 - 1)] - y), 2.0));
                    // If it's a new record for the closest point
                    if ((dist > 0) && (dist <= minDistance))
                    {
                        // Set the closest index and distance to the new record point
                        closestIndex = pixel;
                        minDistance = dist;
                    }
                }
            }

            // If we found a pixel in the search radius
            if (closestIndex >= 0)
            {
                // Add it to the path
                path[pathLength * 2] = pixelsOriginal[closestIndex * 2];
                path[pathLength * 2 + 1] = pixelsOriginal[closestIndex * 2 + 1];
                pathLength++;

                nPix--;
                pixelsOriginal[closestIndex * 2] = pixelsOriginal[nPix * 2];
                pixelsOriginal[closestIndex * 2 + 1] = pixelsOriginal[nPix * 2 + 1];
            }
            // Otherwise, begin a new stroke
            else
            {
                break;
            }
        }
    }
    memmv(pixelsOriginal, path, targetCount * 2 * sizeof(short));
    free(path);
    return 1;
}

void genDPCM(short* buffer, int samples, unsigned char* source, int sourceBytes, float volume)
{
    int value = 0;
    float sampleTimeFactor = (sourceBytes * 8.f) / samples;
    int outS = 0;
    float inS = 0;
    int lastInS = 0;
    int inByte = 0;
    int inBit = 0;
    while (outS < samples) {
        buffer[2 * outS] = f2i(volume * value);
        buffer[2 * outS + 1] = f2i(volume * value);
        outS++;
        inS += sampleTimeFactor;
        if (f2i(inS) > lastInS) {
            inByte = f2i(inS) / 8;
            inBit = f2i(inS) % 8;
            if (source[inByte] & (0b1 << (inBit))) value += 2048;
            else value -= 2048;
            if (value > SHRT_MAX) value = SHRT_MAX;
            if (value < SHRT_MIN) value = SHRT_MIN;
            lastInS = f2i(inS);
        }
    }
}