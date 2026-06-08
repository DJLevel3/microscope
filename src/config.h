//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define XRES        1080
#define YRES        1080

#ifndef STANDALONE
#define FILE_RATE 192000 // Actual sample rate. Changing this changes stuff a lot, including how the drums sound
#define BPM 120.f // BPM. I think you should understand this

// Don't touch these, they adjust the pitches and sample rates to match the BPM
#define SPEED_FACTOR 1
#define SAMPLE_RATE 192000

#define DEMO_SECTIONS 11 // How many sections to put in the demo

#define INTRO_SECTION 0
#define INTRO_SECONDS 8

#define INTRO2_SECTION 1
#define INTRO2_SECONDS 8

#define GREETZ_SECTION 2
#define GREETZ_SECONDS 48

#define TEXT_SECTION 3
#define TEXT_SECONDS 20

#define CUBE_SECTION 4
#define CUBE_SECONDS 32

#define BOUNCY
#define BOUNCY_SECTION 5
#define BOUNCY_SECONDS 44

#define PLASMA_SECTION 6
#define PLASMA_SECONDS 12

#define BALL_SECTION 7
#define BALL_SECONDS 64

#define MID_SECTION 8
#define MID_SECONDS 8

#define PLASMA2_SECTION 9
#define PLASMA2_SECONDS 8

#define OUTRO_SECTION 10
#define OUTRO_SECONDS 8

#define AUDIO_SECONDS (INTRO_SECONDS + INTRO2_SECONDS + GREETZ_SECONDS + TEXT_SECONDS + CUBE_SECONDS + PLASMA_SECONDS + BALL_SECONDS + BOUNCY_SECONDS + MID_SECONDS + PLASMA2_SECONDS + OUTRO_SECONDS)

#define BORDER_SAMPLES 800
#endif

#endif