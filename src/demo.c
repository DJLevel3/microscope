#include "demo.h"
#include "RevisionTraced.h"
//#include "bang.h"
//#include "dammit.h"
//#include "atari.h"
#define ATARI_SIZE 1

float kyoobPointsSrc[] = {
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
};
float kyoobPoints[] = {
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
};

bool intro(short* buffer);

bool textSection(short* buffer, bool easterEgg);
bool textDrums(short* buffer);

bool cube(short* buffer, bool easterEgg);
bool cubeDrums(short* buffer);
bool plasma(short* buffer, bool octave, bool easterEgg);
bool ball(short* buffer, bool easterEgg);
bool ballDrums(short* buffer);
bool greetz(short* buffer, bool easterEgg);
bool midSection(short* buffer, bool easterEgg);
bool outro(short* buffer, bool easterEgg, float freq, int NUMSAMPLES);
bool crash(short* buffer, bool easterEgg);

#ifdef BOUNCY
bool bouncy(short* buffer, bool easterEgg);
#endif

short* kickBuf;
short* snareBuf;
short* openHatBuf;
short* closedHatBuf;
short* crashBuf;
short* riserBuf;
short* pulseSingleBuf;
short* pulseSingleBuf2;
short* pulseSingleBuf3;
float* ballOutlineVerts;
float* revisionVerts3D;
float* atariVerts3D;
short* bangDPCMBuf;
short* dammitBuf;

#define OPENHAT_SAMPLES (48000)
#define CLOSEDHAT_SAMPLES (6000)
#define HAT_RATE (192000)

#define HAT_PERIOD0 (392 * 2)
#define HAT_PERIOD1 (314 * 2)
#define HAT_PERIOD2 (263 * 2)
#define HAT_PERIOD3 (231 * 2)
#define HAT_PERIOD4 (220 * 2)
#define HAT_PERIOD5 (155 * 2)

#define CRASH_SAMPLES 24000
#define RISER_SAMPLES 24000
#define CRASH_RATE 12000

#define BANG_DPCM_SAMPLES 24000
#define DAMMIT_SAMPLES 126000

bool demo_do(long itime, short* buffer, int section, bool easterEgg)
{
    bool b = true;
    if (section == INTRO_SECTION) return intro(buffer);
    if (section == INTRO2_SECTION) {
        b = outro(buffer, easterEgg, mn2f(28), INTRO2_SECONDS * SAMPLE_RATE);
        filterBuffer(buffer, SAMPLE_RATE * INTRO2_SECONDS, 0.0, 0.0f, 0.3f);
        return b;
    }
    if (section == BALL_SECTION) return ball(buffer, easterEgg);
    if (section == CUBE_SECTION) return cube(buffer, easterEgg);
    if (section == PLASMA_SECTION) return plasma(buffer, 0, easterEgg);
    if (section == GREETZ_SECTION) return greetz(buffer, easterEgg);
    if (section == MID_SECTION) return midSection(buffer, easterEgg);
    if (section == PLASMA2_SECTION) return plasma(buffer, 1, easterEgg);
    if (section == TEXT_SECTION) return textSection(buffer, easterEgg);
    if (section == OUTRO_SECTION) {
        b = outro(buffer, easterEgg, mn2f(38), OUTRO_SECONDS * SAMPLE_RATE);
        filterBuffer(buffer, SAMPLE_RATE * 8, 0.0, 1.f, 0.0f);
        fadeBuffer(buffer, 48000, p0d10, p1d00);
        addSamples(buffer, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);
        return b;
    }
#ifdef BOUNCY
    if (section == BOUNCY_SECTION) return bouncy(buffer, easterEgg);
#endif
    memcl(buffer, 0, 2 * 2 * SAMPLE_RATE * sizeof(short));
    return b;
}

float demo_length(int section)
{
    if (section == INTRO_SECTION) return INTRO_SECONDS;
    if (section == INTRO2_SECTION) return INTRO2_SECONDS;
    if (section == BALL_SECTION) return BALL_SECONDS;
    if (section == CUBE_SECTION) return CUBE_SECONDS;
    if (section == PLASMA_SECTION) return PLASMA_SECONDS;
    if (section == MID_SECTION) return MID_SECONDS;
    if (section == TEXT_SECTION) return TEXT_SECONDS;
    if (section == GREETZ_SECTION) return GREETZ_SECONDS;
    if (section == PLASMA2_SECTION) return PLASMA2_SECONDS;
    if (section == OUTRO_SECTION) return OUTRO_SECONDS;
    if (section == BOUNCY_SECTION) return BOUNCY_SECONDS;
    return 2;
}

//---------------------------------------------------------------------
#define NOFF 0

static char oneUpNotes[] = {
    67,
    66,
    63,
    57,
    56,
    64,
    68,
    72
};

static float border[] = {
    -1.f,  1.f, 0.f,
    -1.f, -1.f, 0.f,
     1.f, -1.f, 0.f,
     1.f,  1.f, 0.f,
    -1.f,  1.f, 0.f,
    -1.f, -1.f, 0.f,
};

const static char cubeIndices[] = {
    0, 1, 2, 3, 0,
    5, 6, 7, 4, 5,
    5, 2, 3, 4, 5,
    0, 1, 6, 7, 0,
    0, 3, 4, 7, 0,
    5, 6, 1, 2, 5,
};

static float cubeVerts[] = {
 -1.0f, -1.0f,  1.0f, // 0
 -1.0f,  1.0f,  1.0f, // 1
 -1.0f,  1.0f, -1.0f, // 2
 -1.0f, -1.0f, -1.0f, // 3

  1.0f, -1.0f, -1.0f, // 4
  1.0f,  1.0f, -1.0f, // 5
  1.0f,  1.0f,  1.0f, // 6
  1.0f, -1.0f,  1.0f, // 7
};

#define TEXT_N_PITCHES 64
#define TEXT_N_TIMES 1
static const float textNoteTimes[TEXT_N_TIMES] = {
    0.125
};
static const char textNotes[TEXT_N_PITCHES] = {
    64,
    59,
    64,
    66,
    67,
    59,
    64,
    66,
    69,
    59,
    67,
    66,
    59,
    64,
    66,
    67,
    64,
    59,
    64,
    66,
    67,
    59,
    67,
    69,
    71,
    59,
    69,
    67,
    59,
    66,
    67,
    66,
    64,
    60,
    64,
    66,
    67,
    60,
    66,
    67,
    69,
    60,
    67,
    66,
    60,
    64,
    66,
    67,
    66,
    59,
    63,
    64,
    66,
    59,
    66,
    67,
    69,
    59,
    71,
    66,
    59,
    64,
    66,
    67,
};
static const char textNotes2[4] = {
    55,
    54,
    52,
    51
};
static const char textNotes3[4] = {
    28,
    38,
    36,
    35,
};

#define BALL_OUTLINE_SEGMENTS 32
#define BALL_OUTLINE_VERTS (BALL_OUTLINE_SEGMENTS + 1)
#define BALL_OUTLINE_SEGMENT_ANGLE (2 * M_PI_F / (BALL_OUTLINE_SEGMENTS - 1))

#define BALL_ANGLE -p0d42
#define BALL_ANGLE_DELTA p0d05
#define BALL_X_SEGS 4
#define BALL_Y_SEGS 3

#define BALL_RADIUS_START p0d25
#define BALL_RADIUS_END p0d40

#define BALL_SPEED_X (-860.f / SAMPLE_RATE)
#define BALL_ACC (64.f / SAMPLE_RATE)

#define BALL_N_PITCHES 64
#define BALL_N_TIMES 1
static const float ballNoteTimes[BALL_N_TIMES] = {
    0.125
};

static const char ballNotes[BALL_N_PITCHES] = {
    38,
    50,
    50,
    50,
    50,
    50,
    50,
    36,
    38,
    50,
    50,
    50,
    50,
    50,
    50,
    38,
    39,
    51,
    51,
    51,
    51,
    51,
    51,
    38,
    39,
    51,
    51,
    51,
    51,
    51,
    51,
    39,

    38,
    50,
    50,
    50,
    50,
    50,
    50,
    36,
    38,
    50,
    50,
    50,
    50,
    50,
    50,
    38,
    39,
    51,
    51,
    51,
    51,
    51,
    51,
    51,
    53,
    53,
    53,
    53,
    54,
    54,
    54,
    54,
};

static const unsigned short ball2Fades = 0b0111111001111110;

#define CUBE_N_PITCHES 16
#define CUBE_N_TIMES 1
static const float cubeNoteTimes[CUBE_N_PITCHES] = {
    0.125
};

static const char cubeNotes[CUBE_N_PITCHES] = {
    38,
    50,
    38,
    50,
    51,
    39,
    51,
    39,
    51,
    50,
    38,
    50,
    51,
    39,
    51,
    39,
};

#define FINALE_N_PITCHES 12
#define FINALE_N_TIMES 6
static const float finaleNoteTimes[FINALE_N_PITCHES] = {
    1.5,
    1,
    1.5,
    1.5,
    1,
    1.5,
};
static const char finaleNotes[FINALE_N_PITCHES] = {
    38,
    NOFF,
    40,
    41,
    NOFF,
    45,
    50,
    NOFF,
    44,
    43,
    NOFF,
    40
};

float twisterHeight;
float rotMatrix1[16];
static int seed = 0;

#define PULSE_SAMPLES SAMPLE_RATE / 4

int demo_init( int itime )
{
    kickBuf = (short*)malloc(sizeof(short) * KICK_SAMPLES * 2);
    if (kickBuf == 0) return 0;

    snareBuf = (short*)malloc(sizeof(short) * SNARE_SAMPLES * 2);
    if (snareBuf == 0) goto RETA;

    openHatBuf = (short*)malloc(sizeof(short) * OPENHAT_SAMPLES * 2);
    if (openHatBuf == 0) goto RETB;

    closedHatBuf = (short*)malloc(sizeof(short) * CLOSEDHAT_SAMPLES * 2);
    if (closedHatBuf == 0) goto RETC;

    crashBuf = (short*)malloc(sizeof(short) * CRASH_SAMPLES * 2);
    if (crashBuf == 0) goto RETD;
    /*
    pulse1Buf = (short*)malloc(sizeof(short) * PULSE_SAMPLES * 2);
    if (pulse1Buf == 0) goto RETE;

    pulse2Buf = (short*)malloc(sizeof(short) * PULSE_SAMPLES * 2);
    if (closedHatBuf == 0) goto RETF;

    pulse3Buf = (short*)malloc(sizeof(short) * PULSE_SAMPLES * 2);
    if (closedHatBuf == 0) goto RETG;

    pulse4Buf = (short*)malloc(sizeof(short) * PULSE_SAMPLES * 2);
    if (closedHatBuf == 0) goto RETH;
    */
    ballOutlineVerts = (float*)malloc(sizeof(float) * 3 * BALL_OUTLINE_VERTS);
    if (ballOutlineVerts == 0) goto RETI;

    riserBuf = (short*)malloc(sizeof(short) * RISER_SAMPLES * 2);
    if (riserBuf == 0) goto RETJ;

    pulseSingleBuf = (short*)malloc(sizeof(short) * PULSE_SAMPLES);
    if (pulseSingleBuf == 0) goto RETK;

    pulseSingleBuf2 = (short*)malloc(sizeof(short) * PULSE_SAMPLES / 2);
    if (pulseSingleBuf2 == 0) goto RETL;

    pulseSingleBuf3 = (short*)malloc(sizeof(short) * PULSE_SAMPLES * 4);
    if (pulseSingleBuf3 == 0) goto RETM;
    
    atariVerts3D = (float*)malloc(sizeof(float) * 3 * ATARI_SIZE);
    if (atariVerts3D == 0) goto RETN;
#ifdef USE_REVISION
    revisionVerts3D = genRevision();
#else
    revisionVerts3D = (float*)malloc(sizeof(float) * 3);
#endif
    if (revisionVerts3D == 0) goto RETO;

    bangDPCMBuf = (short*)malloc(sizeof(short) * 2 * BANG_DPCM_SAMPLES);
    if (bangDPCMBuf == 0) goto RETP;

    dammitBuf = (short*)malloc(sizeof(short) * 2 * DAMMIT_SAMPLES);
    if (dammitBuf == 0) {
    RETQ:
        free(bangDPCMBuf);
    RETP:
        free(revisionVerts3D);
    RETO:
        free(atariVerts3D);
    RETN:
        free(pulseSingleBuf3);
    RETM:
        free(pulseSingleBuf2);
    RETL:
        free(pulseSingleBuf);
    RETK:
        free(riserBuf);
    RETJ:
        free(ballOutlineVerts);
    RETI:
        /*
        free(pulse4Buf);
        RETH:
        free(pulse3Buf);
        RETG:
        free(pulse2Buf);
        RETF:
        free(pulse1Buf);
        RETE:*/
        free(crashBuf);
    RETD:
        free(closedHatBuf);
    RETC:
        free(openHatBuf);
    RETB:
        free(snareBuf);
    RETA:
        free(kickBuf);
        return 0;
    }

    seed = itime;

    // kick
    float kFreq = KICK_FMAX;
    float kSPC;
    int kSamp;
    for (int i = 0; i < KICK_SAMPLES;) {
        kSPC = FILE_RATE / kFreq;
        for (kSamp = 0; kSamp < kSPC && i < KICK_SAMPLES; kSamp++) {
            kickBuf[i * 2] = f2i(SHRT_MAX * squareWaveCentered(kSamp, f2i(kSPC), p0d60));
            kickBuf[i * 2 + 1] = f2i(SHRT_MAX * squareWaveCentered(kSamp + f2i(kSPC * 0.25f), f2i(kSPC), p0d60));
            i++;
        }
        if (kFreq > KICK_FBASE) kFreq -= KICK_FSTEP;
        else kFreq = KICK_FBASE;
    }
    fadeBuffer(kickBuf, KICK_SAMPLES, p1d50, p1d50);
    fadeBuffer(kickBuf, KICK_FADE_START, KICK_START_V, KICK_START_V);
    fadeBuffer(kickBuf + (KICK_FADE_START * 2), KICK_FADE_MID - KICK_FADE_START, KICK_START_V, KICK_MID_V);
    fadeBuffer(kickBuf + (KICK_FADE_MID * 2), KICK_FADE_END - KICK_FADE_MID, KICK_MID_V, KICK_END_V);
    fadeBuffer(kickBuf + (KICK_FADE_END * 2), KICK_SAMPLES - KICK_FADE_END, KICK_END_V, KICK_END_V);

    // snare
    float sFreq = SNARE_FMAX;
    float sSPC;
    int sSamp;
    float n;
    for (int i = 0; i < SNARE_SAMPLES; ) {
        sSPC = FILE_RATE / sFreq;
        sSamp = 0;
        for (sSamp = 0; sSamp < sSPC && i < SNARE_SAMPLES; sSamp++) {
            if (sSamp % 7 == 0) n = demo_rand(&seed) * powf(min(max((float)i - SNARE_WIREWAIT, 0), (float)SNARE_WIRETIME) / (SNARE_WIRETIME), 2.0f);
            snareBuf[i * 2] = f2i(0.5f * SHRT_MAX * squareWaveNoDC(sSamp, f2i(sSPC), p0d40) + n);
            snareBuf[i * 2 + 1] = snareBuf[i * 2];
            i++;
        }
        if (sFreq > SNARE_FBASE) sFreq -= SNARE_FSTEP;
        else sFreq = SNARE_FBASE;
    }
    fadeBuffer(snareBuf, SNARE_SAMPLES, p0d80, p0d80);
    fadeBuffer(snareBuf + SNARE_SAMPLES, SNARE_SAMPLES / 2, 1.f, 0.f);

    // hihat
    float state1 = 0;
    float state2 = 0;
    //float state3 = 0;
    float retval;
    for (int i = 0; i < OPENHAT_SAMPLES; i++) {
#ifdef NOISE_HIHAT
        retval = short(demo_rand(&seed));
#else
        retval = 10922.f * (
            squareWaveNoDC(i, HAT_PERIOD0, 0.5f) +
            squareWaveNoDC(i, HAT_PERIOD1, 0.5f) +
            squareWaveNoDC(i, HAT_PERIOD2, 0.5f) +
            squareWaveNoDC(i, HAT_PERIOD3, 0.5f) +
            squareWaveNoDC(i, HAT_PERIOD4, 0.5f) +
            squareWaveNoDC(i, HAT_PERIOD5, 0.5f)
            );
#endif
        state1 += p0d37 * 0.5 * (retval + state1 - 2 * state2);
        state2 += p0d37 * 0.5 * (state1 - state2);
        //state3 += p0d45 * (state2 - state3);
        openHatBuf[i * 2] = f2i(retval - state2);
        openHatBuf[i * 2 + 1] = openHatBuf[i * 2];
        if (i < CLOSEDHAT_SAMPLES) {
            closedHatBuf[i * 2] = openHatBuf[i * 2];
            closedHatBuf[i * 2 + 1] = openHatBuf[i * 2];
        }
    }
    fadeBuffer(openHatBuf, OPENHAT_SAMPLES, p0d65, p0d00);
    fadeBuffer(closedHatBuf, CLOSEDHAT_SAMPLES, p0d65, p0d00);

    short x, y;
    float d;
    for (int i = 0; i < CRASH_SAMPLES; i++) {
        do {
            x = demo_rand(&seed);
            y = demo_rand(&seed);
            d = sqrtf((int)(x)*x + (int)(y)*y);
        } while (d > SHRT_MAX);
        crashBuf[i * 2] = x; 
        crashBuf[i * 2 + 1] = y;

        riserBuf[i * 2] = demo_rand(&seed);
        riserBuf[i * 2 + 1] = demo_rand(&seed);
    }
    fadeBuffer(crashBuf, CRASH_SAMPLES, p0d30, p0d00);
    fadeBuffer(riserBuf, RISER_SAMPLES, p0d00, p0d50);
    filterBuffer(riserBuf, RISER_SAMPLES, p0d85, 0, p0d65);

    memcl(pulseSingleBuf, 0, PULSE_SAMPLES * sizeof(short));
    {
        for (int s = 0; s < PULSE_SAMPLES / 2; s++) {
            pulseSingleBuf[s * 2] = f2i(SHRT_MAX * (0.5f + squareWaveCentered(s, f2i(SAMPLE_RATE / mn2f(55)), p0d85)));
            pulseSingleBuf[s * 2 + 1] = 0;
        }
    }
    fadeBuffer(pulseSingleBuf, PULSE_SAMPLES / 2, 0.6f, 0.f);

    memcl(pulseSingleBuf2, 0, PULSE_SAMPLES / 2 * sizeof(short));
    {
        for (int s = 0; s < PULSE_SAMPLES / 4; s++) {
            pulseSingleBuf2[s * 2] = 0;
            pulseSingleBuf2[s * 2 + 1] = f2i(SHRT_MAX * (0.5f + squareWaveCentered(s, f2i(SAMPLE_RATE / mn2f(55)), p0d85)));
        }
    }
    fadeBuffer(pulseSingleBuf2, PULSE_SAMPLES / 4, 0.6f, 0.f);

    memcl(pulseSingleBuf3, 0, PULSE_SAMPLES * 4 * sizeof(short));
    {
        for (int s = 0; s < PULSE_SAMPLES * 2; s++) {
            pulseSingleBuf3[s * 2] = f2i(SHRT_MAX * squareWaveCentered(s, f2i(SAMPLE_RATE / mn2f(68)), p0d85));
            pulseSingleBuf3[s * 2 + 1] = f2i(SHRT_MAX * squareWaveCentered(s, f2i(SAMPLE_RATE / mn2f(68)), p0d85));
        }
    }
    fadeBuffer(pulseSingleBuf3, PULSE_SAMPLES * 2, 0.3f, 0.f);

    return 1;
}

void demo_end() {
    free(kickBuf);
    free(snareBuf);
    free(openHatBuf);
    free(closedHatBuf);
    free(crashBuf);
    free(ballOutlineVerts);
    free(pulseSingleBuf);
    free(pulseSingleBuf2);
    free(pulseSingleBuf3);
    free(riserBuf);
    free(atariVerts3D);
    free(revisionVerts3D);
    free(bangDPCMBuf);
    free(dammitBuf);
}

const unsigned short kickPattern =  0b1000001000100100;
const unsigned int hatPattern =     0b00000000010000000000000001000100;
const unsigned int hatRollPattern = 0b00000001000000000010000100000000;

const char* sayIt[2] = { "SAY IT", "ATA RI" };
bool ball(short* buffer, bool easterEgg) {
    int DEMO_NUMSAMPLES = f2i(BALL_SECONDS * SAMPLE_RATE);
    int s;
    float nextTime = 0;
    int lastTime = 0;
    int p = 0;
    float freq = 110;
    float nSPC, nSPS;

    const int nStrokes = 16;
    freq = mn2f(cubeNotes[0]); // TODO - yes I intended to have the first note be an octave up
    nSPC = SAMPLE_RATE / freq;
    nSPS = nSPC / nStrokes;
    nextTime += SAMPLE_RATE * cubeNoteTimes[p];

    float* line = (float*)malloc(sizeof(float) * 6);
    if (line == 0) {
        memcl(buffer, 0, DEMO_NUMSAMPLES * 2);
        return false;
    }
    line[2] = 0;
    line[5] = 0;

    int counter = 0;
    double prog = 0;
    double targetProg = 0;
    int samples;
    int samplesCorrected;
    float timer = 0;
    float scroller = 0;

    float ballPosX = -p0d40;
    float ballPosY = p0d50;
    float ballVelY = 0;
    float ballVelX = BALL_SPEED_X;

    float ballRadius = BALL_RADIUS_START;
    int sFac;
    s = 0;

    int pos = 0;

    int now;
    float nowX;
    float nowY;
    bool set = false;

    for (; s < DEMO_NUMSAMPLES;) {
        demo_rand(&seed);
        if (counter == ((s >= SAMPLE_RATE * 8) * BALL_Y_SEGS) + 1) {
            targetProg += 6 * nSPS;
            samples = f2i(targetProg - prog);
            prog += samples;
            if (s < 32 * SAMPLE_RATE) {
                now = min(4, max(0, s - 6 * SAMPLE_RATE) * 2 / SAMPLE_RATE);
                gridBuffer(buffer + 2 * s, samples, 1, now);
                now = max(0, s - 16 * SAMPLE_RATE);
                nowX = min(now / 8.f / SAMPLE_RATE, 1.f);
                wobbleBufferEnv(buffer + 2 * s, samples, SAMPLE_RATE, now % SAMPLE_RATE, 1.f, 1.f, 0.1f * nowX, 0.1f * nowX, 0.5f);
                punchBuffer(buffer + 2 * s, samples, f2i(ballPosX * SHRT_MAX), f2i(ballPosY * SHRT_MAX), ballRadius);
            }
            else if (pos < 0.078125f * SAMPLE_RATE || !(ball2Fades & 1 << (15 - (p % 16))) || s >= 48 * SAMPLE_RATE) {
                nowX = (float)(s) / SAMPLE_RATE - 40.f;
                nowX = max(0.0f, nowX) / 4.f;
                for (int i = 0; i < 5; i++) {
                    line[1] = sinf(M_PI_F * 2 / 5 * i + M_PI_F * nowX);
                    line[4] = line[1];
                    line[0] = -1.f;
                    line[3] = 1.f;
                    lineToSamples(line, line + 3, buffer + (s + samples * i / 10) * 2, samples / 10);
                }

                for (int i = 5; i < 10; i++) {
                    line[0] = sinf(M_PI_F * 2 / 5 * i - M_PI_F * nowX);
                    line[3] = line[0];
                    line[1] = -1.f;
                    line[4] = 1.f;
                    lineToSamples(line, line + 3, buffer + (s + samples * i / 10) * 2, i == 9 ? samples - (samples / 10 * 9) : samples / 10);
                }
                if (s >= 48 * SAMPLE_RATE) wobbleBufferEnv(buffer + 2 * s, samples, SAMPLE_RATE / 2.f, s % SAMPLE_RATE, 1.f, 1.f, 0.1f, 0.1f, 0.5f);
                punchBuffer(buffer + 2 * s, samples, f2i(ballPosX * SHRT_MAX), f2i(ballPosY * SHRT_MAX), ballRadius);
            }
            counter = (counter + 6) % nStrokes;
            s += samples;
            pos += samples;
        }
        else if (counter == ((s >= SAMPLE_RATE * 8) * BALL_Y_SEGS)) {
            targetProg += nSPS;
            samples = f2i(targetProg - prog);
            prog += samples;
            ballOutlineVerts[0] = -ballRadius;
            ballOutlineVerts[1] = 0.f;
            ballOutlineVerts[2] = 0.f;
            for (int i = 1; i < BALL_OUTLINE_VERTS; i++) {
                rotZ(ballOutlineVerts, ballOutlineVerts + 3 * i, i * BALL_OUTLINE_SEGMENT_ANGLE);
            }
            if (true) { //s < 32 * SAMPLE_RATE || (s % (SAMPLE_RATE * 2) < SAMPLE_RATE)) {
                strokeToCycle2D(ballOutlineVerts, BALL_OUTLINE_VERTS, buffer + 2 * s, samples);
            }
            else {
                // nothing lol
            }
            offsetBuffer(buffer + 2 * s, samples, f2i(ballPosX * SHRT_MAX), f2i(ballPosY * SHRT_MAX));
            counter = (counter + 1) % nStrokes;
            s += samples;
            pos += samples;
        }
        else if (s >= SAMPLE_RATE * 8 && counter < BALL_Y_SEGS) {
            targetProg += nSPS;
            samples = f2i(targetProg - prog);
            prog += samples;
            if (s < 32 * SAMPLE_RATE || (s % (SAMPLE_RATE * 2) < SAMPLE_RATE)) {
                line[0] = -ballRadius;
                line[1] = -ballRadius + (((counter) + 0.5) * ballRadius * 2 / (BALL_Y_SEGS));
                line[3] = ballRadius;
                line[4] = line[1];
                strokeToCycle2D(line, 2, buffer + 2 * s, samples);
                addShade(buffer + 2 * s, samples, 3, 0, ballRadius / 2 / BALL_Y_SEGS);
                offsetBufferCheckered(buffer + 2 * s, samples, 0, f2i(SHRT_MAX * ballRadius / BALL_Y_SEGS), samples / BALL_X_SEGS, f2i((1.f - fmodf(scroller, 1.f)) * samples / BALL_X_SEGS), 0.5f);
                if (easterEgg) fadeBuffer(buffer + 2 * s, samples, p1d20, p1d20);
                else ballify(buffer + 2 * s, samples, ballRadius, true);
                rotateBuffer(buffer + 2 * s, samples, BALL_ANGLE);
                offsetBuffer(buffer + 2 * s, samples, f2i(ballPosX * SHRT_MAX), f2i(ballPosY * SHRT_MAX));
            }
            else {
                strokeToCycle2D(revisionVerts3D + 3 * revisionOffsets[counter * 2], revisionLengths[counter * 2], buffer + 2 * s, samples);
                strokeToCycle2D(revisionVerts3D + 3 * revisionOffsets[counter * 2 + 1], revisionLengths[counter * 2 + 1], buffer + 2 * (s + samples), samples);
                fadeBuffer(buffer + 2 * s, samples * 2, ballRadius * p0d90, ballRadius * p0d90);
                if (easterEgg) {
                    wobbleBufferEnv(buffer + 2 * s, samples * 2, SAMPLE_RATE / 2, s, 2, 3, 0.1f, 0.1f, 0.1f);
                    fadeBuffer(buffer + 2 * s, samples * 2, p0d70, p0d70);
                }
                rotateBuffer(buffer + 2 * s, samples * 2, M_PI_F + scroller * M_PI_F / -2.f);
                offsetBuffer(buffer + 2 * s, samples * 2, f2i(ballPosX * SHRT_MAX), f2i(ballPosY * SHRT_MAX));
                s += samples;
                pos += samples;
                prog += samples;
                targetProg += nSPS;
            }
            s += samples;
            counter = (counter + 1) % nStrokes;
            pos += samples;
        }
        else {
            sFac = s >= SAMPLE_RATE * 8 ? 9 - BALL_Y_SEGS : 9;
            targetProg += nSPS * ((s >= SAMPLE_RATE * 32) && (s % (SAMPLE_RATE * 2) >= SAMPLE_RATE) ? sFac - 3 : sFac);
            samples = f2i(targetProg - prog);
            prog += samples;
            samplesCorrected = f2i(samples * (s >= 12 * SAMPLE_RATE && s < 16 * SAMPLE_RATE) / 18.f);
            for (int i = 0; i < 6 && samplesCorrected > 0; i++) {
                if (sayIt[0][i] != ' ') {
                    drawChar(buffer + 2 * s, samplesCorrected, sayIt[easterEgg][i], (i - 3) * 0.3f, -0.125f + sinf(i * 0.6f + 4.f * s / SAMPLE_RATE) / 15, (i - 3) * 0.3f + 0.25f, 0.125f + sinf(i * 0.6f + 4.f * s / SAMPLE_RATE) / 15);
                    s += samplesCorrected;
                }
            }
            if (s < 32 * SAMPLE_RATE || pos < 0.078125f * SAMPLE_RATE || !(ball2Fades & 1 << (15 - (p % 16))) || s >= 48 * SAMPLE_RATE) {
                strokeToCycle2D(border, 6, buffer + 2 * s, samples - 5 * samplesCorrected);
            }
            counter = (counter + sFac) % nStrokes;
            s += samples - 5 * samplesCorrected;
            pos += samples;
        }

        if (counter == 0) {
            float xAcc = -1.f;
            if (s < SAMPLE_RATE * 32) {
                ballRadius = ((float)(s) / SAMPLE_RATE) - 8;
                ballRadius = BALL_RADIUS_START + ((max(0, ballRadius) / (BALL_SECONDS - 8.f)) * (BALL_RADIUS_END - BALL_RADIUS_START));
                if (s > SAMPLE_RATE * 24) {
                    ballRadius += p0d10 * sinf(s * M_PI_F / SAMPLE_RATE / 2);
                }
                xAcc = -1.1f;
            }
            timer += 1.f / freq;
            prog -= nSPC;
            targetProg -= nSPC;
            ballPosX += ballVelX;
            ballPosY += ballVelY;
            if (ballPosX > 1.f - ballRadius) {
                ballPosX -= (ballPosX + ballRadius - 1.f);
                ballVelX = xAcc * ballVelX;
            }
            if (ballPosX < -1.f + ballRadius) {
                ballPosX -= (ballPosX - ballRadius + 1.f);
                ballVelX = xAcc * ballVelX;
            }
            if (ballPosY < -1.f + ballRadius) {
                ballPosY -= (ballPosY - ballRadius + 1.f);
                ballVelY = -ballVelY;
            }
            if (ballPosY > 1.f - ballRadius) {
                ballPosY -= (ballPosY + ballRadius - 1.f);
                ballVelY = -ballVelY;
            }
            ballVelY -= BALL_ACC;
            scroller = fmodf((scroller + (ballVelX * BALL_ANGLE_DELTA / fabsf(ballVelX))) + 4.f, 4.f);
        }

        while (s > nextTime) {
            pos = 0;
            prog = 0;
            targetProg = 0;
            p++;
            if (s < 32 * SAMPLE_RATE) {
                nextTime += SAMPLE_RATE * cubeNoteTimes[p % CUBE_N_TIMES];
                freq = mn2f(cubeNotes[p % CUBE_N_PITCHES] - 12);
            } else
            {
                nextTime += SAMPLE_RATE * ballNoteTimes[p % BALL_N_TIMES];
                freq = mn2f(ballNotes[p % BALL_N_PITCHES] - 12);
            }
            nSPC = SAMPLE_RATE / freq;// -BORDER_SAMPLES;
            nSPS = nSPC / nStrokes;
            lastTime = s;
        }

        if (GetAsyncKeyState(VK_ESCAPE)) {
            free(line);
            return false;
        }
    }

    fadeBuffer(buffer, DEMO_NUMSAMPLES, p0d45, p0d45);
    free(line);

    return ballDrums(buffer);
}
bool ballDrums(short* buffer) {
    fadeBuffer(buffer, 48000, p0d10, p1d00);
    addSamples(buffer, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);
    addSamples(buffer, crashBuf, CRASH_SAMPLES, FILE_RATE / CRASH_RATE);
    addSamples(buffer + SAMPLE_RATE * 30 * 2, riserBuf, RISER_SAMPLES, SAMPLE_RATE / CRASH_RATE);
    addSamples(buffer + SAMPLE_RATE * 32 * 2, crashBuf, CRASH_SAMPLES, FILE_RATE / CRASH_RATE);
    //return 1;

    int i;
    bool didntKick;
#define KICKS4F 0b1000
#define KICKSPAT 0b0100
#define SNARES 0b0010
#define HATS 0b0001
    // 1000 kicks(4-on-the-floor)
    // 0100 kicks (pattern)
    // 0010 snares
    // 0001 hats
    short parts = 0;

    for (i = 0; i < 512; i++) {
        if (i == 48 || i == 65) parts ^= KICKS4F;
        if (i == 125) parts ^= KICKSPAT + SNARES;
        if (i == 192) parts ^= HATS;
        if (i == 256) parts = 0;
        if (i == 316) parts ^= SNARES;
        if (i == 320) parts ^= KICKSPAT + HATS;
        if (i == 480) parts ^= KICKS4F + KICKSPAT;
        didntKick = true;
        if (((i % 4 == 0) && (parts & KICKS4F)) || ((kickPattern & 1 << (15 - (i % 16))) && (parts & KICKSPAT))) {
            didntKick = false;
            fadeBuffer(buffer + ((SAMPLE_RATE >> 3) * i) * 2, 48000, p0d10, p1d00);
            addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);
        }
        if (i % 8 == 4 && (parts & SNARES)) {
            if (didntKick) fadeBuffer(buffer + ((SAMPLE_RATE >> 3) * i) * 2, 36000, p0d50, p1d00);
            addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, snareBuf, SNARE_SAMPLES, FILE_RATE / SNARE_RATE);
        }
        if ((parts & HATS)) {
            if (hatRollPattern & (1 << (31 - (i % 32)))) {
                addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, closedHatBuf, min(CLOSEDHAT_SAMPLES, SAMPLE_RATE >> 4), FILE_RATE / HAT_RATE);
                addSamples(buffer + ((SAMPLE_RATE >> 3) * i + (SAMPLE_RATE >> 4)) * 2, closedHatBuf, min(CLOSEDHAT_SAMPLES, SAMPLE_RATE >> 5), FILE_RATE / HAT_RATE);
            }
            else if (hatPattern & (1 << (31 - (i % 32)))) {
                addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, openHatBuf, min(OPENHAT_SAMPLES, SAMPLE_RATE >> 3), FILE_RATE / HAT_RATE);
            }
            else {
                addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, closedHatBuf, min(CLOSEDHAT_SAMPLES, SAMPLE_RATE >> 3), FILE_RATE / HAT_RATE);
            }
        }
        if (i % 10 == 0 && GetAsyncKeyState(VK_ESCAPE)) {
            return false;
        }
    }
    return true;
}

bool cube(short* buffer, bool easterEgg) {
    int DEMO_NUMSAMPLES = f2i(CUBE_SECONDS * SAMPLE_RATE);
    int s;
    float nextTime = 0;
    int p = 0;
    int nStrokes;
    int faces;
    int curFace = 0;
    float freq, nSPC, nSPS;

    nStrokes = 0;
    faces = curFace;
    while (faces == curFace) {
        demo_rand(&seed);
        demo_rand(&seed);
        demo_rand(&seed);
        faces = (demo_rand(&seed) & 0b1111) % 6;
    }
    curFace = faces;
    faces = 0b111111111111 - (0b11 << (faces * 2));
    nStrokes = 1;

    freq = mn2f(cubeNotes[0]);
    if (cubeNotes[p % CUBE_N_PITCHES] == NOFF) freq = 110;
    nSPC = SAMPLE_RATE / freq - BORDER_SAMPLES;
    nSPS = nSPC / nStrokes;
    nextTime += SAMPLE_RATE * cubeNoteTimes[p % CUBE_N_PITCHES];

    int counter = 0;
    double prog = 0;
    double targetProg = 0;
    int samples;
    float timer = 0;

    s = 0;

    float* currentStroke = (float*)malloc(sizeof(float)* (3 * 5));
    if (currentStroke == 0) {
        return false;
    }
    
    for (; s < DEMO_NUMSAMPLES;) {
        demo_rand(&seed);
        targetProg += nSPS;
        samples = f2i(targetProg - prog);
        prog += samples;
        int mask = (0b11 << (counter * 2));
        if ((nStrokes == 6 || ((faces & mask) == false)) && (cubeNotes[p % CUBE_N_PITCHES] != NOFF)) {
            for (int v = 0; v < 5; v++) {
                currentStroke[v * 3 + 0] = cubeVerts[3 * cubeIndices[counter * 5 + v] + 0];
                currentStroke[v * 3 + 1] = cubeVerts[3 * cubeIndices[counter * 5 + v] + 1];
                currentStroke[v * 3 + 2] = cubeVerts[3 * cubeIndices[counter * 5 + v] + 2];
                rotX(currentStroke + v * 3, currentStroke + v * 3, sinf(M_PI_F * timer / 2) * M_PI_F + easterEgg * s * 4.f / SAMPLE_RATE);
                rotY(currentStroke + v * 3, currentStroke + v * 3, M_PI_F * timer + easterEgg * s * 4.f / SAMPLE_RATE);
                rotZ(currentStroke + v * 3, currentStroke + v * 3, M_PI_F * timer + easterEgg * s * 4.f / SAMPLE_RATE);
            }
            strokeToCycle3D(currentStroke, 5, buffer + 2 * s, samples, view_matrix);
            s += samples;
        }

        counter = (counter + 1) % 6;
        if (counter == 0) {
            prog -= nSPC;
            targetProg -= nSPC;
            if (cubeNotes[p % CUBE_N_PITCHES] != NOFF) {
                timer += 1.f / freq;
                strokeToCycle2D(border, 6, buffer + 2 * s, BORDER_SAMPLES);
            }
            s += BORDER_SAMPLES;
        }

        while (s > nextTime) {
            p++;
            prog = 0;
            targetProg = 0;

            if (s >= 16 * SAMPLE_RATE) {
                nStrokes = 6;
            }
            else if (s >= 8 * SAMPLE_RATE) {
                nStrokes = 0;
                while (nStrokes == 0) {
                    faces = demo_rand(&seed) & 0b111111111111;
                    for (int i = 0; i < 6; i++) {
                        if ((faces & (3 << (i * 2))) == false) {
                            nStrokes++;
                        }
                    }
                }
            }
            else if (p % 4 == 0) {
                faces = curFace;
                while (faces == curFace) {
                    demo_rand(&seed);
                    demo_rand(&seed);
                    demo_rand(&seed);
                    faces = (demo_rand(&seed) & 0b1111) % 6;
                }
                curFace = faces;
                faces = 0b111111111111 - (0b11 << (faces * 2));
                nStrokes = 1;
            }

            // check pitch and shit
            freq = mn2f(cubeNotes[p % CUBE_N_PITCHES] - 12);
            if (cubeNotes[p % CUBE_N_PITCHES] == NOFF) freq = 110;
            nSPC = SAMPLE_RATE / freq - BORDER_SAMPLES;
            nSPS = nSPC / nStrokes;
            nextTime += SAMPLE_RATE * cubeNoteTimes[p % CUBE_N_TIMES];
        }

        if (GetAsyncKeyState(VK_ESCAPE)) {
            free(currentStroke);
            return false;
        }
    }

    free(currentStroke);

    wobbleBufferEnv(buffer + 24 * SAMPLE_RATE * 2, SAMPLE_RATE * 8, SAMPLE_RATE / 1.5f, 0, 2.f / 3, 0.f, 0.12f, 0.0f, 0.5f);
    wobbleBufferEnv(buffer + 24 * SAMPLE_RATE * 2, SAMPLE_RATE * 8, SAMPLE_RATE / 2.0f, 0, 0.f, 2.f / 3, 0.0f, 0.12f, 0.5f);
    fadeBuffer(buffer, DEMO_NUMSAMPLES, p0d45, p0d45);

    if (GetAsyncKeyState(VK_ESCAPE)) {
        return false;
    }
    
    bool b = cubeDrums(buffer);
    return b;
}
bool cubeDrums(short* buffer){
    addSamples(buffer, crashBuf, CRASH_SAMPLES, FILE_RATE / CRASH_RATE);
    fadeBuffer(buffer, 48000, p0d10, p1d00);
    addSamples(buffer, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);
    addSamples(buffer + SAMPLE_RATE * 30 * 2, riserBuf, RISER_SAMPLES, SAMPLE_RATE / CRASH_RATE);

    int i;

    // sidechain
    for (i = 128; i < 256; i++) {
        if (i % 4 == 0) {
            fadeBuffer(buffer + ((SAMPLE_RATE >> 3) * i) * 2, 36000, p0d10, p1d00);
            addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);
        }
        if (i >= 192 && i % 8 == 4) {
            addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, snareBuf, SNARE_SAMPLES, FILE_RATE / SNARE_RATE);
        }
        if (i > 64) {
            if (i % 16 == 10) addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, openHatBuf, min(OPENHAT_SAMPLES, SAMPLE_RATE >> 3), FILE_RATE / HAT_RATE);
            else addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, closedHatBuf, CLOSEDHAT_SAMPLES, FILE_RATE / HAT_RATE);
        }
    }

    return true;
}

unsigned char plasmaFunction(short x, short y, float t)
{
    float xN = 1.5 * (float)(x) / (SHRT_MAX);
    float yN = 1.5 * (float)(y) / (SHRT_MAX);
    float xyz[] = { xN, yN, 0 };
    rotZ(xyz, xyz, M_PI_F * t * 0.25f);
    xyz[0] += t * 0.5f;
    float x2 = xyz[0] + 2.f;
    float y2 = xyz[1] + 2.f;

    float f = cos(1.2f * sin(t - 3.f * sqrtf(x2 * x2 + y2 * y2)))
        * (sin(xyz[0] * 4.0f + t)
        - 1.3f * cos(xyz[1] - 2.0f * t)
        + sin(t) * sin(xyz[0] * 1.5334f + 1.232f * sin(t) + t) * sin(xyz[1] * 3.0f + xyz[0] * cos(t)));
    return f2i(255 * min(1.f, max(0.f, 1.f - pow(fabsf(f), 1.0f))));
}
unsigned char pf2(short x, short y, float iTime)
{
    float xN = (float)(x) / (SHRT_MAX) / 3;
    float yN = (float)(y) / (SHRT_MAX) / 4;
    float xyz[] = { xN, yN, 0 };
    rotZ(xyz, xyz, (0.2f + xN * sinf(iTime / 5.f) * 0.1f - yN * cosf((iTime + 2.f) * 6.f / 3.f) * 0.5f) * iTime);
    float x2 = xyz[0] + sinf(iTime) - sinf(iTime * 3.2f + xyz[0] * 8.f) / 4.f;
    float y2 = xyz[1] + sinf(10.f * xyz[1]) - cos(iTime / 4.f) / 3.f;

    float f = sinf(x2 * 12.f + iTime) + sinf(y2 * 2.f - 3.f * iTime) + cosf((x2 + y2) * 5.f + iTime);
    return f2i(255 * min(1.f, max(0.f, 1.f - powf(fabsf(f), 4.0f))));
}
bool plasma(short* buffer, bool octave, bool easterEgg) {
    int DEMO_NUMSAMPLES = f2i(((octave ? PLASMA2_SECONDS : PLASMA_SECONDS)) * SAMPLE_RATE);
    int s;
    int nStrokes;
    int faces;
    float freq, nSPC, nSPS;

    nStrokes = 1;

    float startFreq = octave ? 26 : 8;
    float endFreq = 26;
    float dF = (endFreq - startFreq);
    freq = mn2f(startFreq);
    nSPC = SAMPLE_RATE / freq;
    nSPS = nSPC / nStrokes - BORDER_SAMPLES;

    int counter = 0;
    double prog = 0;
    double targetProg = 0;
    int samples;
    float timer = 0;
    float hilliTimer = 0;

    s = 0;

    for (; s < DEMO_NUMSAMPLES;) {
        demo_rand(&seed);
        targetProg += nSPS;
        samples = f2i(targetProg - prog);
        prog += samples;

        short* marker = buffer + 2 * s;

        if (!hilligoss(marker, samples / 2, demo_rand(&seed), hilliTimer, 1.f / SAMPLE_RATE, octave ? plasmaFunction : pf2, 1)) {
            return false;
        }
        if (easterEgg) {
            fadeBuffer(buffer + 2 * s, samples / 2, p0d95 * sqrtf(0.5f), p0d95 * sqrtf(0.5f));
            rotateBuffer(buffer + 2 * s, samples / 2, (float)(s) / SAMPLE_RATE * s / SAMPLE_RATE);
        }
        s += samples / 2;
        strokeToCycle2D(border, 6, buffer + 2 * s, BORDER_SAMPLES / 2);
        s += BORDER_SAMPLES / 2;

        memmv(buffer + 2 * s, marker, (samples / 2 + BORDER_SAMPLES / 2) * 2 * sizeof(short));
        s += samples / 2;
        s += BORDER_SAMPLES / 2;

        timer += (1.f / freq);
        hilliTimer += (1.f / freq) + (float)(octave) / 165.f;

        // check pitch and shit
        freq = mn2f(startFreq + dF * min(1.f, (s * 16.f / (16.f  - 2 * octave) / DEMO_NUMSAMPLES))) * (1 + easterEgg * (1 - octave)) + (easterEgg * 8) * (1 - octave) * sinf(M_PI_F * s * 3.f / SAMPLE_RATE);
        nSPC = SAMPLE_RATE / freq;
        nSPS = nSPC / nStrokes - BORDER_SAMPLES;

        if (GetAsyncKeyState(VK_ESCAPE)) {
            return false;
        }
    }
    
    fadeBuffer(buffer, DEMO_NUMSAMPLES, p0d45, p0d45);

    if (!octave) {
        filterBuffer(buffer + (SAMPLE_RATE >> 1) * 46, (SAMPLE_RATE >> 1), 0.45, 1, 0);

        for (int i = 8; i < 24; i++) {
            fadeBuffer(buffer + ((SAMPLE_RATE >> 1) * i) * 2, 36000, p0d10, p1d00);
            addSamples(buffer + ((SAMPLE_RATE >> 1) * i) * 2, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);
        }
        addSamples(buffer + (SAMPLE_RATE >> 2) * 47 * 2, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);
        addSamples(buffer + SAMPLE_RATE * (10) * 2, riserBuf, RISER_SAMPLES, SAMPLE_RATE / CRASH_RATE);
        filterBuffer(buffer, DEMO_NUMSAMPLES / 2, 0.55, 0.f, 1.f);
    }
    else {
        filterBuffer(buffer, DEMO_NUMSAMPLES, 0.55, 0.8, 0.3f);
    }
    fadeBuffer(buffer, 48000, p0d10, p1d00);
    addSamples(buffer, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);

    return true;
}

const char bec[] = "BBBECCCC";
const unsigned int introMorse = 0b11101010100010001110101110100000;
bool intro(short* buffer) {
    int i;
    int n;
    int c;

    int nStrokes = 12;
    float nSPC = SAMPLE_RATE / mn2f(28);
    float nSPS = nSPC / nStrokes;
    float targetProg = 0;
    float prog = 0;
    int samples = 0;
    c = 0;
    
    int s = 0;
    for (; s < 8*SAMPLE_RATE;) {
        demo_rand(&seed);
        targetProg += nSPC / 2;
        samples = f2i(targetProg - prog);
        prog += samples;
        bool m = (introMorse & (1 << 31 - (s * 16 / SAMPLE_RATE % 32)));

        if (m) drawChar(buffer + 2 * s, samples, bec[c % 8], -1.f, -1.f, 1.f, 1.f);

        s += samples;
        c = (s * 4 / SAMPLE_RATE);
    }

    fadeBuffer(buffer, 8 * SAMPLE_RATE, 0.0f, p0d45);

    return true;
}

static const char* textCharacters[2][2] = {
    { " THIS IS   *NOT*  A VECTREX  DEMO   ",
      "LOAD DEMO         READY    RUN?     "},
    { " THIS IS   *NOT*  A MIGAAAA  AAAA   ",
      "OSCI MUZK         ROCKS    LMAO     "}
};
bool textSection(short* buffer, bool easterEgg)
{
    int DEMO_NUMSAMPLES = f2i(TEXT_SECONDS * SAMPLE_RATE);
    int s;
    float nextTime = 0;
    int p = 0;
    float freq = 110;
    float nSPC, nSPS;
    int c = 0;
    float posX;
    float posY;

    const int nStrokes = 32;
    freq = mn2f(textNotes3[0]);
    nSPC = SAMPLE_RATE / freq;
    nSPS = nSPC / nStrokes;
    nextTime += SAMPLE_RATE * 2;

    int counter = 0;
    double prog = 0;
    double targetProg = 0;
    int samples;
    float timer = 0;

    s = 0;
    int sFac;
    for (; s < DEMO_NUMSAMPLES;) {
        demo_rand(&seed);
        targetProg += nSPS;
        samples = f2i(targetProg - prog);
        prog += samples;

        sFac = (s >= 16 * SAMPLE_RATE ? 19 : 9) - min(max(0, (s * 3 / 2 / SAMPLE_RATE - 23)), 3);
        sFac -= (s * 4 / SAMPLE_RATE % 2 == 0) * (s >= 18 * SAMPLE_RATE);
        if (counter < sFac) {
            strokeToCycle2D(border, 6, buffer + 2 * s, samples * sFac);
            counter += sFac - 1;
            targetProg += nSPS * (sFac - 1);
            prog += samples * (sFac - 1);
            s += samples * sFac;
        }
        else {
            while (textCharacters[0][s >= 16 * SAMPLE_RATE][c] == ' ') c = (c + 1) % 36;
            posX = ((c % 9) + (s < 16 * SAMPLE_RATE ? c / 27 : 0) / 2.f) / 8.f - 0.5625f + 0.0125;
            posY = (c / 9) / -4.f + 0.25f + 0.0625f + 0.0125;
            drawChar(buffer + 2 * s, samples, textCharacters[easterEgg][s >= 16 * SAMPLE_RATE][c], (posX) * 1.5f, (posY) * 1.5f, (posX + p0d10) * 1.5f, (posY + p0d10) * 1.5f);
            s += samples;
            c = (c + 1) % 36;
        }

        counter = (counter + 1) % nStrokes;
        if (counter == 0) {
            timer += 1.f / freq;
            prog -= nSPC;
            targetProg -= nSPC;
            c = 0;
        }

        while (s > nextTime && s < SAMPLE_RATE * 16) {
            prog = 0;
            targetProg = 0;
            p++;
            nextTime += SAMPLE_RATE * 2;
            freq = mn2f(textNotes3[p % 4]);
            nSPC = SAMPLE_RATE / freq;
            nSPS = nSPC / nStrokes;
        }

        if (GetAsyncKeyState(VK_ESCAPE)) {
            return false;
        }
    }

    fadeBuffer(buffer, DEMO_NUMSAMPLES, p0d45, p0d45);
    filterBuffer(buffer, 4 * SAMPLE_RATE, 0.0f, 0.05f, 0.1f);
    filterBuffer(buffer + 8 * SAMPLE_RATE, 8 * SAMPLE_RATE, 0.0f, 0.1f, 0.5f);
    filterBuffer(buffer + 24 * SAMPLE_RATE, 4 * SAMPLE_RATE, 0.0f, 0.5f, 1.f);

    return textDrums(buffer);
}
bool textDrums(short* buffer)
{
    addSamples(buffer, crashBuf, CRASH_SAMPLES, FILE_RATE / CRASH_RATE);
    fadeBuffer(buffer, 48000, p0d10, p1d00);

    for (int i = 0; i < 128; i++) {
        addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, pulseSingleBuf2, PULSE_SAMPLES / 4, mn2f(55) / mn2f(textNotes2[(i / 16) % 4]));
        addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, pulseSingleBuf, PULSE_SAMPLES / 2, mn2f(55) / mn2f(textNotes[i % TEXT_N_PITCHES]));
    }
    for (int i = 128; i < 160; i++) {
        addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, pulseSingleBuf2, PULSE_SAMPLES / 4, mn2f(55) / mn2f(textNotes2[3]));
        addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, pulseSingleBuf, PULSE_SAMPLES / 2, mn2f(55) / mn2f(textNotes[i % 16 + 48]));
    }

    addSamples(buffer, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);
    addSamples(buffer + SAMPLE_RATE * 37, riserBuf, RISER_SAMPLES, SAMPLE_RATE / CRASH_RATE);
    return true;
}

static const char* greetsText[] = {
    "READY     C?        ", // 0
    "READY     CR?       ", // 1
    "READY     CRE?      ", // 2
    "READY     CRED?     ", // 3
    "READY     CREDI?    ", // 4
    "READY     CREDIT?   ", // 5
    "READY     CREDITS?  ", // 6
    "READY     CREDITS?  ", // 7
    "BUS  ERRORCOLLECTIVE", // 8
    " PRESENTS           ", // 10
    " PRESENTS   READY?  ", // 12
    " PRESENTS   READY?  ", // 14
    "  A 16KB  EXECUTABLE", // 16
    "OSCI MUSIC   DEMO   ", // 18
    "                    ", // 20
    "   OOPS   ONE SECOND", // 22
    "  LET ME  CHECK THAT", // 24
    "                    ", // 26
    "APPARENTLYWE ALREADY", // 28
    " RELEASED    THIS   ", // 30
    " BUT WAIT  A SECOND ", // 32
    "NOW MY GPU IS MAXED ", // 34
    " WHAT IS   GOING ON ", // 36
    " WHAT IS   GOING ON ", // 38
    "MICROSCOPEC?        ", // 40
    "MICROSCOPECR?       ", // 41
    "MICROSCOPECRE?      ", // 42
    "MICROSCOPECRED?     ", // 43
    "MICROSCOPECREDI?    ", // 44
    "MICROSCOPECREDIT?   ", // 45
    "MICROSCOPECREDITS?  ", // 46
    "MICROSCOPECREDITS?  ", // 47
    "BUS  ERRORCOLLECTIVE", // 48
    " PRESENTS           ", // 50
    " PRESENTS MICROSCOPE", // 52
    " PRESENTS MICROSCOPE", // 54
    "A SOFTWARE  OSCOPE  ", // 56
    "  IN 3KB  STANDALONE", // 58
    "  OR NET    2KB ON  ", // 60
    " THE 13KB   READY?  ", // 62
    " NO EXTRA  HARDWARE ", // 64
    " NO EXTRA  SOFTWARE ", // 66
    "NO AI CODE INVOLVED ", // 68
    "ALL SHADER WIZARDRY ", // 70
    "WRITTEN BYDJ_LEVEL_3", // 72
    " FOR NOVA    2026   ", // 74
    "   OPEN     SOURCE  ", // 76
    " AND JUST  REMEMBER ", // 78
    " WHAT YOU   SEE IS  ", // 80
    " WHAT YOU    HEAR   ", // 82
    "ENJOY  THE SHOWCASE ", // 84
    "ENJOY  THE SHOWCASE ", // 86
    "READY     LOADING?  ", // 88
    "READY     ?         ",
};
bool greetz(short* buffer, bool easterEgg)
{
    int s;
    float nextTime = 0;
    int p = 0;
    int c = 0;
    float posX;
    float posY;

    const int nStrokes = 24;
    const float nSPC = SAMPLE_RATE / mn2f(28);
    const float nSPS = nSPC / nStrokes; 
    nextTime += SAMPLE_RATE * 2;

    int counter = 0;
    double prog = 0;
    double targetProg = 0;
    int samples;
    float timer = 0;

    s = 0;
    int sFac;
    bool drawn;
    char character;
    for (; s < GREETZ_SECONDS * SAMPLE_RATE;) {
        demo_rand(&seed);
        targetProg += nSPS;
        samples = f2i(targetProg - prog);
        prog += samples;

        posX = ((c % 10) * (10 / 9.f) - 5.f) / 9.f - 0.05f;
        posY = (c / 10) * -0.2f + 0.05f;
        counter = (s * 2) / SAMPLE_RATE;
        if (counter < 8) character = counter;
        else if (counter < 40) character = (counter / 2) + 4;
        else if (counter < 48) character = counter - 16;
        else if (counter < 90) character = (counter / 2) + 8;
        else character = 52;
        character = greetsText[character][c];
        drawChar(buffer + 2 * s, samples, (character != '?') || ((s * 4 / SAMPLE_RATE) % 2 == 0) ? character : ' ', (posX) * 1.5f, (posY) * 1.5f, (posX + p0d10) * 1.5f, (posY + p0d10) * 1.5f);
        if (easterEgg && character != '?') {
            rotateBuffer(buffer + 2 * s, samples, -p0d10);
            if (character != ' ') offsetBuffer(buffer + 2 * s, samples, 0, -8192);
        }
        s += samples;
        c = (c + 1) % 20;
        if (c == 0) {
            if (counter < 20 || counter >= 40) strokeToCycle2D(border, 6, buffer + 2 * s, samples * 4);
            s += samples * 4;
            prog += samples * 4;
            targetProg += nSPS * 4;
        }
    }

    memcl(buffer + 20 * SAMPLE_RATE, 0, SAMPLE_RATE * sizeof(short));
    
    fadeBuffer(buffer, GREETZ_SECONDS * SAMPLE_RATE, p0d45, p0d45);
    secretFound(buffer + 20 * SAMPLE_RATE);

    filterBuffer(buffer + 36 * SAMPLE_RATE, 2 * SAMPLE_RATE, 0.0f, 1.0f, 0.0f);
    filterBuffer(buffer + 40 * SAMPLE_RATE, 4 * SAMPLE_RATE, 0.0f, 0.0f, 1.0f);

    unsigned int bechat = 0b1011101010110100;
    // hihats
    int i;
    for (i = 0; i <= 80; i++) {
        if (i % 16 == 6) addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, openHatBuf, OPENHAT_SAMPLES, FILE_RATE / HAT_RATE);
        else if (bechat & (0b1000000000000000 >> (i % 16))) addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, closedHatBuf, CLOSEDHAT_SAMPLES, FILE_RATE / HAT_RATE);
    }
    for (i = 0; i <= 184; i++) {
        if (i % 16 == 6) addSamples(buffer + (40 * SAMPLE_RATE) + ((SAMPLE_RATE >> 3) * i) * 2, openHatBuf, OPENHAT_SAMPLES, FILE_RATE / HAT_RATE);
        else if (bechat & (0b1000000000000000 >> (i % 16))) addSamples(buffer + (40 * SAMPLE_RATE) + ((SAMPLE_RATE >> 3) * i) * 2, closedHatBuf, CLOSEDHAT_SAMPLES, FILE_RATE / HAT_RATE);
    }

    filterBuffer(buffer, SAMPLE_RATE, 0.0f, 0.3f, 1.f);
    /*
    filterBuffer(buffer + 50 * SAMPLE_RATE, SAMPLE_RATE, 0.3, 1.0f, 0.5f);
    filterBuffer(buffer + 52 * SAMPLE_RATE, SAMPLE_RATE, 0.3, 0.5f, 0.25f);
    filterBuffer(buffer + 54 * SAMPLE_RATE, SAMPLE_RATE, 0.3, 0.25f, 0.0f);*/
    //filterBuffer(buffer + 48 * SAMPLE_RATE, SAMPLE_RATE * 4, 0.0, 1.f, 0.0f);

    //fadeBuffer(buffer, 48000, p0d10, p1d00);
    //addSamples(buffer, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);

    addSamples(buffer + 92 * SAMPLE_RATE, riserBuf, RISER_SAMPLES, SAMPLE_RATE / CRASH_RATE);

    return true;
}

bool midSection(short* buffer, bool easterEgg) {
    int DEMO_NUMSAMPLES = f2i(MID_SECONDS * SAMPLE_RATE);
    int s;
    float nextTime = 0;
    int p = 0;
    int nStrokes;
    int faces;
    int curFace = 0;
    float freq, nSPC, nSPS;

    nStrokes = 0;
    faces = curFace;
    while (faces == curFace) {
        demo_rand(&seed);
        demo_rand(&seed);
        demo_rand(&seed);
        faces = (demo_rand(&seed) & 0b1111) % 6;
    }
    curFace = faces;
    faces = 0b111111111111 - (0b11 << (faces * 2));
    nStrokes = 1;

    freq = mn2f(cubeNotes[0]);
    if (cubeNotes[p % CUBE_N_PITCHES] == NOFF) freq = 110;
    nSPC = SAMPLE_RATE / freq - BORDER_SAMPLES;
    nSPS = nSPC / nStrokes;
    nextTime += SAMPLE_RATE * cubeNoteTimes[p % CUBE_N_PITCHES];

    int counter = 0;
    double prog = 0;
    double targetProg = 0;
    int samples;
    float timer = 0;

    s = 0;

    float* currentStroke = (float*)malloc(sizeof(float) * (3 * 5));
    if (currentStroke == 0) {
        return false;
    }

    for (; s < DEMO_NUMSAMPLES;) {
        demo_rand(&seed);
        targetProg += nSPS;
        samples = f2i(targetProg - prog);
        prog += samples;
        int mask = (0b11 << (counter * 2));
        if ((faces & mask) == false || nStrokes == 6) {
            for (int v = 0; v < 5; v++) {
                currentStroke[v * 3 + 0] = cubeVerts[3 * cubeIndices[counter * 5 + v] + 0];
                currentStroke[v * 3 + 1] = cubeVerts[3 * cubeIndices[counter * 5 + v] + 1];
                currentStroke[v * 3 + 2] = cubeVerts[3 * cubeIndices[counter * 5 + v] + 2];
                rotX(currentStroke + v * 3, currentStroke + v * 3, sinf(M_PI_F * timer / 2) * M_PI_F);
                rotY(currentStroke + v * 3, currentStroke + v * 3, M_PI_F * timer);
                rotZ(currentStroke + v * 3, currentStroke + v * 3, M_PI_F * timer);
            }
            strokeToCycle3D(currentStroke, 5, buffer + 2 * s, samples, view_matrix);
            s += samples;
        }

        counter = (counter + 1) % 6;
        if (counter == 0) {
            prog -= nSPC;
            targetProg -= nSPC;
            if (cubeNotes[p % CUBE_N_PITCHES] != NOFF) {
                timer += 1.f / freq;
                strokeToCycle2D(border, 6, buffer + 2 * s, BORDER_SAMPLES);
            }
            s += BORDER_SAMPLES;
        }

        while (s > nextTime) {
            p++;
            prog = 0;
            targetProg = 0;
            
            nStrokes = 0;
            while (nStrokes == 0) {
                faces = demo_rand(&seed) & 0b111111111111;
                for (int i = 0; i < 6; i++) {
                    if ((faces & (3 << (i * 2))) == false) {
                        nStrokes++;
                    }
                }
            }
            if (s > SAMPLE_RATE * 6) {
                nStrokes = 6;
            }
            

            // check pitch and shit
            freq = mn2f(cubeNotes[p % CUBE_N_PITCHES] - 12 * (s < SAMPLE_RATE * 4));
            nSPC = SAMPLE_RATE / freq - BORDER_SAMPLES;
            nSPS = nSPC / nStrokes;
            nextTime += SAMPLE_RATE * cubeNoteTimes[p % CUBE_N_TIMES];
        }

        if (GetAsyncKeyState(VK_ESCAPE)) {
            free(currentStroke);
            return false;
        }
    }

    free(currentStroke);

    wobbleBufferEnv(buffer, SAMPLE_RATE * 6, SAMPLE_RATE / 1.5f, 0, 2.f / 3, 0.f, 0.12f, 0.0f, 0.5f);
    wobbleBufferEnv(buffer, SAMPLE_RATE * 6, SAMPLE_RATE / 2.0f, 0, 0.f, 2.f / 3, 0.0f, 0.12f, 0.5f);
    if (easterEgg) wobbleBufferEnv(buffer, DEMO_NUMSAMPLES, SAMPLE_RATE / 4, 0, 2.f, 2.f, 0.1f, 0.1f, 0.1f);
    fadeBuffer(buffer, DEMO_NUMSAMPLES, p0d45, p0d45);

    for (s = 0; s < 4; s++) {
        fadeBuffer(buffer + 2 * s * SAMPLE_RATE * 2, 48000, p0d10, p1d00);
        addSamples(buffer + 2 * s * SAMPLE_RATE * 2, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);
    }
    for (int i = 0; i < 32; i++) {
        if (i % 16 == 13) addSamples(buffer + ((SAMPLE_RATE >> 2) * i) * 2, openHatBuf, min(OPENHAT_SAMPLES, SAMPLE_RATE >> 2), FILE_RATE / HAT_RATE);
        else if (i % 8 == 3) {
            addSamples(buffer + ((SAMPLE_RATE >> 2) * i) * 2, closedHatBuf, min(CLOSEDHAT_SAMPLES, SAMPLE_RATE >> 3), FILE_RATE / HAT_RATE);
            addSamples(buffer + (SAMPLE_RATE >> 2) + ((SAMPLE_RATE >> 2) * i) * 2, closedHatBuf, min(CLOSEDHAT_SAMPLES, SAMPLE_RATE >> 3), FILE_RATE / HAT_RATE);
        }
        else addSamples(buffer + ((SAMPLE_RATE >> 2) * i) * 2, closedHatBuf, min(CLOSEDHAT_SAMPLES, SAMPLE_RATE >> 2), FILE_RATE / HAT_RATE);
    }

    if (GetAsyncKeyState(VK_ESCAPE)) {
        return false;
    }
    return true;
}

bool outro(short* buffer, bool easterEgg, float freq, int NUMSAMPLES) {
    int s;
    int p = 0;
    int nStrokes = 24;
    float nSPC = SAMPLE_RATE / freq;
    float nSPS = nSPC / nStrokes;
    int c = 0;
    float posX;
    float posY;
    double prog = 0;
    double targetProg = 0;
    int samples;

    s = 0;
    char character;
    for (; s - SAMPLE_RATE / 2 < NUMSAMPLES;) {
        demo_rand(&seed);
        targetProg += nSPS;
        samples = f2i(targetProg - prog);
        prog += samples;

        posX = ((c % 10) * (10 / 9.f) - 5.f) / 9.f - 0.05f;
        posY = (c / 10) * -0.2f + 0.05f;
        character = greetsText[53][c];
        drawChar(buffer + 2 * s, samples, (character != '?') || ((s * 4 / SAMPLE_RATE) % 2 == 0) ? character : ' ', (posX) * 1.5f, (posY) * 1.5f, (posX + p0d10) * 1.5f, (posY + p0d10) * 1.5f);
        if (easterEgg) {
            rotateBuffer(buffer + 2 * s, samples, -p0d10);
            if (character != ' ') offsetBuffer(buffer + 2 * s, samples, 0, -8192);
        }
        s += samples;
        c = (c + 1) % 20;
        if (c == 0) {
            strokeToCycle2D(border, 6, buffer + 2 * s, samples * 4);
            s += samples * 4;
            prog += samples * 4;
            targetProg += nSPS * 4;
        }
    }

    fadeBuffer(buffer, NUMSAMPLES, p0d45, p0d45);

    return true;
}

static const char discordNotes[] = {43, 48, 55};
void secretFound(short* buffer)
{
    addSamples(buffer, pulseSingleBuf3, PULSE_SAMPLES / 12, mn2f(56) / mn2f(discordNotes[0]));
    addSamples(buffer + SAMPLE_RATE / 4, pulseSingleBuf3, PULSE_SAMPLES / 8, mn2f(56) / mn2f(discordNotes[1]));
    addSamples(buffer + SAMPLE_RATE / 4, pulseSingleBuf3, PULSE_SAMPLES / 6, mn2f(56) / mn2f(discordNotes[2]));
    filterBuffer(buffer, SAMPLE_RATE / 4, 0.f, p0d10, p0d15);
}

static const char* crashText[] = {
    "                        ",
    "JUST KIDDINGPLASMA TIME ",
};
#ifdef BOUNCY
bool bouncy(short* buffer, bool easterEgg) {
    int p = 0;
#define N_BOUNCERS 32

    int nStrokes = N_BOUNCERS;
    float freq = ballNotes[0] - 12;
    float nSPC = SAMPLE_RATE / freq - 2 * BORDER_SAMPLES;
    float nextTime = ballNoteTimes[0] * SAMPLE_RATE;
    float nSPS = nSPC / nStrokes;
    float targetProg = 0;
    float prog = 0;
    int samples = 0;
    int c = 0;
    float pos = 0;
    float temp;

    float centerX = 0.f;
    float centerY = 0.f;
    float lastCenterX = 0.f;
    float lastCenterY = 0.f;
    float bSize = 0.65f;

    // xPos, yPos, xVel, yVel
    float* states = (float*)malloc(N_BOUNCERS * 4 * sizeof(float));
    if (states == 0) return false;

    for (int i = 0; i < N_BOUNCERS; i++) {
        states[i * 4] = 0.f;
        states[i * 4 + 1] = 0.f;
        states[i * 4 + 2] = (float)demo_rand(&seed) / SAMPLE_RATE / SHRT_MAX * N_BOUNCERS * 4;
        states[i * 4 + 3] = (float)demo_rand(&seed) / SAMPLE_RATE / SHRT_MAX * N_BOUNCERS * 2;
    }

    int s = 0;
    for (; s < 32 * SAMPLE_RATE;) {
        demo_rand(&seed);
        targetProg += nSPS;
        samples = f2i(targetProg - prog);
        prog += samples;

        if ((c < s * 2.f / SAMPLE_RATE && !easterEgg) && (pos < p0d06 * SAMPLE_RATE || !(ball2Fades & 1 << (15 - (p % 16))))) {
            for (int i = 0; i < samples; i++) {
                states[4 * c] += states[4 * c + 2] * freq / mn2f(38);
                if (states[4 * c] > centerX + bSize) { // hit right wall
                    states[4 * c] = 2 * (centerX + bSize) - states[4 * c];
                    states[4 * c + 2] = -states[4 * c + 2] + (centerX - lastCenterX) / nSPC;
                }
                else if (states[4 * c] < centerX - bSize) { // hit left wall
                    states[4 * c] = 2 * (centerX - bSize) - states[4 * c];
                    states[4 * c + 2] = -states[4 * c + 2] + (centerX - lastCenterX) / nSPC;
                }

                states[4 * c + 1] += states[4 * c + 3] * freq / mn2f(38);
                if (states[4 * c + 1] > centerY + bSize) { // hit top wall
                    states[4 * c + 1] = 2 * (centerY + bSize) - states[4 * c + 1];
                    states[4 * c + 3] = -states[4 * c + 3] + (centerY - lastCenterY) / nSPC;
                }
                else if (states[4 * c + 1] < centerY - bSize) { // hit bottom wall
                    states[4 * c + 1] = 2 * (centerY - bSize) - states[4 * c + 1];
                    states[4 * c + 3] = -states[4 * c + 3] + (centerY - lastCenterY) / nSPC;
                }
                states[4 * c + 3] -= 0.003 * N_BOUNCERS / SAMPLE_RATE;
                vec2ToSample(states + 4 * c, buffer + 2 * (s + i));
            }
            fadeBuffer(buffer + 2 * s, samples, p0d45, p0d45);
        }

        s += samples;
        pos += samples;
        c = (c + 1) % N_BOUNCERS;

        if (c == 0) {
            strokeToCycle2D(border, 6, buffer + 2 * s, BORDER_SAMPLES >> 1);
            fadeBuffer(buffer + 2 * s, BORDER_SAMPLES >> 1, bSize, bSize);
            offsetBuffer(buffer + 2 * s, BORDER_SAMPLES >> 1, f2i(centerX * SHRT_MAX), f2i(centerY * SHRT_MAX));
            if ((pos < p0d06 * SAMPLE_RATE || !(ball2Fades & 1 << (15 - (p % 16))))) strokeToCycle2D(border, 6, buffer + 2 * (s + (BORDER_SAMPLES >> 1)), (3 * BORDER_SAMPLES) >> 1);
            fadeBuffer(buffer + 2 * s, 2 * BORDER_SAMPLES, p0d45, p0d45);
            s += BORDER_SAMPLES * 2;
            pos += BORDER_SAMPLES * 2;


            lastCenterX = centerX;
            temp = max(0, s - 16 * SAMPLE_RATE);
            centerX = p0d29 * sinf(M_PI_F * temp * 3.f / SAMPLE_RATE) * sinf(M_PI_F * s * 0.25f / SAMPLE_RATE);

            lastCenterY = centerY;
            temp = max(0, s - 8 * SAMPLE_RATE);
            centerY = p0d15 * sinf(M_PI_F * temp * 4.f / SAMPLE_RATE) * cosf(M_PI_F * s * 0.25f / SAMPLE_RATE);
        }

        while (s > nextTime) {
            p++;
            prog = 0;
            pos = 0;
            targetProg = 0;

            // check pitch and shit
            freq = mn2f(ballNotes[p % BALL_N_PITCHES] - 12);
            nSPC = SAMPLE_RATE / freq - 2 * BORDER_SAMPLES;
            nSPS = nSPC / nStrokes;
            nextTime += SAMPLE_RATE * ballNoteTimes[p % BALL_N_TIMES];
        }

        if (GetAsyncKeyState(VK_ESCAPE)) {
            free(states);
            return false;
        }
    }
    free(states);


    fadeBuffer(buffer, 48000, p0d10, p1d00);
    addSamples(buffer, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);
    addSamples(buffer, crashBuf, CRASH_SAMPLES, FILE_RATE / CRASH_RATE);

    int i;
    bool didntKick;
#define KICKS4F 0b1000
#define KICKSPAT 0b0100
#define SNARES 0b0010
#define HATS 0b0001
    // 1000 kicks(4-on-the-floor)
    // 0100 kicks (pattern)
    // 0010 snares
    // 0001 hats
    short parts = SNARES;

    for (i = 0; i < 256; i++) {
        if (i == 56 || i == 65) parts ^= KICKS4F;
        if (i == 128) parts ^= KICKS4F;
        if (i == 192) parts ^= HATS;
        didntKick = true;
        if (((i % 4 == 0) && (parts & KICKS4F)) || ((kickPattern & 1 << (15 - (i % 16))) && (parts & KICKSPAT))) {
            didntKick = false;
            fadeBuffer(buffer + ((SAMPLE_RATE >> 3) * i) * 2, 48000, p0d10, p1d00);
            addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, kickBuf, KICK_SAMPLES, FILE_RATE / KICK_RATE);
        }
        if (i % 8 == 4 && (parts & SNARES)) {
            if (didntKick) fadeBuffer(buffer + ((SAMPLE_RATE >> 3) * i) * 2, 36000, p0d50, p1d00);
            addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, snareBuf, SNARE_SAMPLES, FILE_RATE / SNARE_RATE);
        }
        if ((parts & HATS)) {
            if (hatRollPattern & (1 << (31 - (i % 32)))) {
                addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, closedHatBuf, min(CLOSEDHAT_SAMPLES, SAMPLE_RATE >> 4), FILE_RATE / HAT_RATE);
                addSamples(buffer + ((SAMPLE_RATE >> 3) * i + (SAMPLE_RATE >> 4)) * 2, closedHatBuf, min(CLOSEDHAT_SAMPLES, SAMPLE_RATE >> 5), FILE_RATE / HAT_RATE);
            }
            else if (hatPattern & (1 << (31 - (i % 32)))) {
                addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, openHatBuf, min(OPENHAT_SAMPLES, SAMPLE_RATE >> 3), FILE_RATE / HAT_RATE);
            }
            else {
                addSamples(buffer + ((SAMPLE_RATE >> 3) * i) * 2, closedHatBuf, min(CLOSEDHAT_SAMPLES, SAMPLE_RATE >> 3), FILE_RATE / HAT_RATE);
            }
        }
        if (i % 10 == 0 && GetAsyncKeyState(VK_ESCAPE)) {
            return false;
        }
    }

    // crash some shit
    s = f2i(31.15 * SAMPLE_RATE);
    while (s < BOUNCY_SECONDS * SAMPLE_RATE) {
        memmv(buffer + 2 * s, buffer + 64 * SAMPLE_RATE - 9424, 908 * 2 * sizeof(short));
        s += 983;
    }

    const int nChars = 24;
    // wipe the screen or something I don't fucking know
    int deleted_counter;
    int sTwo;
    s = 31 * SAMPLE_RATE;
    while (s < BOUNCY_SECONDS * SAMPLE_RATE) {
        deleted_counter = 0;
        sTwo = s;
        // fuck shit up

        for (int c = 0; c < 5230; c++) {
            if (buffer[2 * (s)+1] > f2i(SHRT_MAX * (40.f * SAMPLE_RATE - s) / (2.f * SAMPLE_RATE))) {
                deleted_counter += 1;
            }
            else {
                buffer[2 * (sTwo)] = buffer[2 * (s)];
                buffer[2 * (sTwo)+1] = buffer[2 * (s)+1];
                sTwo++;
            }
            s++;
        }

        memcl(buffer + 2 * (sTwo), 0, 2 * deleted_counter * sizeof(short));
        for (int i = 0; i < nChars; i++) {
            float xPos = ((((i % 12)) / 11.f) - 0.5f);
            float yPos = (0.5f - (i / 12)) * 0.25f;
            drawChar(buffer + 2 * (sTwo), deleted_counter / nChars, crashText[s / (float)SAMPLE_RATE > 40.5][i], xPos - 0.04f, yPos - 0.04f, xPos + 0.04f, yPos + 0.04f);
            sTwo += (deleted_counter) / nChars;
        }
    }


    return true;
}
#endif
