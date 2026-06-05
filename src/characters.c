#include "characters.h"

static const char characterVerts[] = {
    -10, -10, // S (overlaps C/U)
     10, -10,
     10,  0,
    -10,  0,
    -10,  10,
     10,  10, // C/U (overlaps S/R)
    -5,   10,
    -10,  5,
    -10, -5,
    -5,  -10,
     10, -10, // R (overlaps C/U/A/V/P)
    -10,  0,  // P (overlaps R/A/V)
     10,  0,
     10,  10,
    -10,  10,
    -10, -10, // A/V (overlaps R/P/B)
     0,   10,
     10, -10,
     5,   0,
    -5,   0,
    -10, -10, // B (overlaps A/V/M/W)
    -10,  10,
     6,   10,
     10,  6,
     10,  2,
     8,   0,
    -10,  0,
     8,   0,
     10, -2,
     10, -6,
     6,  -10,
    -10, -10, // M/W (overlaps B)
    -10,  6,
    -6,   10,
    -4,   10,
     0,   6,
     0,   0,
     0,   6,
     4,   10,
     6,   10,
     10,  6,
     10, -10, // H/I (overlaps M/W/I/Z/N)
    -10, -10,
     0,  -10,
     0,   10,
     10,  10,
    -10,  10, // Z/N (overlaps H/I/E)
     10,  10,
    -10, -10,
     10, -10,
    -10, -10,
     10,  10,
    -10,  10,
     10,  10, // E (overlaps Z/N/F/T)
    -10,  10,
    -10, -10,
     10, -10,
    -10, -10, // F (overlaps E)
    -10,  0,
     10,  0,
    -10,  0,
    -10,  10,
     10,  10,
    -10,  10, // T (overlaps F/E)
     10,  10,
     0,   10,
     0,  -10,
    -10,  10, // L
    -10, -10,
     10, -10,
    -10,  5,  // O
    -5,   10,
     5,   10,
     10,  5,
     10, -5,
     5,  -10,
    -5,  -10,
    -10, -5,
    -10,  5,
    -5,   10,
    -10, -10, // K (overlaps X)
    -10,  10,
    -10,  0,
     10,  10,
    -10,  0,
     10, -10, // X (overlaps K, D)
    -10,  10,
     0,   0,
     10,  10,
    -10, -10, // D (overlaps X, Y)
    -10,  10,
     5,   10,
     10,  5,
     10, -5,
     5,  -10,
    -10, -10,
    -10,  10, // Y (overlaps D)
     0,   0,
     10,  10,
     0,   0,
     0,  -10,
     0,   0,  // G
     10,  0,
     10, -5,
     5,  -10,
    -5,  -10,
    -10, -5,
    -10,  5,
    -5,   10,
     5,   10,
     10,  5,
    -10,  10, // J
     10,  10,
     0,   10,
     0,  -5,
    -5,  -10,
    -10, -5,
     0,   0, // *
     0,   10, 
     0,  -10,
     0,   0,
    -8,  -6,
     8,   6,
     0,   0,
     8,  -6,
    -8,   6,
     0,   0,
    -10,  0, // 6/8
     10,  0,
     10, -10,
    -10, -10,
    -10,  10,
     10,  10,
     10,  0,
    -8,   10, // ?
     8,   10,
     8,  -10,
    -8,  -10,
    -8,   10,
    -10,  10, // 4
    -10,  0,
     10,  0,
     10,  10,
     10, -10
};
static const char vertCounts[39] = {
    6,  // A
    12, // B
    6,  // C
    8,  // D
    11, // E
    6,  // F
    10, // G
    6,  // H
    6,  // I
    6,  // J
    6,  // K
    3,  // L
    11, // M
    8,  // N
    10, // O
    5,  // P
    0,  // Q
    6,  // R
    6,  // S
    4,  // T
    6,  // U
    3,  // V
    11, // W
    5,  // X
    5,  // Y
    8,  // Z
    10, // 0
    2,  // 1
    6,  // 2
    11, // 3
    5,  // 4
    6,  // 5
    6,  // 6
    3,  // 7
    7,  // 8
    6,  // 9
    10, // *
    5,  // ?
    2,  // _
};

// swap, flipX, flipY (applied in that order)
static const char vertFlipsSwaps[39] = {
    0b000, // A
    0b000, // B
    0b000, // C
    0b000, // D
    0b000, // E
    0b000, // F
    0b000, // G
    0b100, // H
    0b000, // I
    0b000, // J
    0b000, // K
    0b000, // L
    0b000, // M
    0b101, // N
    0b000, // O
    0b000, // P
    0b000, // Q
    0b000, // R
    0b000, // S
    0b000, // T
    0b110, // U
    0b001, // V
    0b001, // W
    0b000, // X
    0b000, // Y
    0b000, // Z
    0b000, // 0
    0b000, // 1
    0b010, // 2
    0b010, // 3
    0b000, // 4
    0b000, // 5
    0b000, // 6
    0b100, // 7
    0b000, // 8
    0b011, // 9
    0b000, // *
    0b000, // _
};

static const unsigned char vertOffsets[39] = {
    15,  // A
    20,  // B
    5,   // C
    89,  // D
    53,  // E
    57,  // F
    101, // G
    41,  // H
    41,  // I
    111, // J
    80,  // K
    67,  // L
    31,  // M
    46,  // N
    70,  // O
    11,  // P
    0,   // Q
    10,  // R
    0,   // S
    63,  // T
    5,   // U
    15,  // V
    31,  // W
    85,  // X
    96,  // Y
    46,  // Z
    70,  // 0
    43,   // 1
    0,   // 2
    53,   // 3
    139,   // 4
    0,   // 5
    127, // 6
    67,  // 7
    127, // 8
    127, // 9
    117, // *
    134, // ?
    0,   // _
};

void drawChar(short* buffer, int samples, char character, float x1, float y1, float x2, float y2)
{
    float offX = (x1 + x2) / 2;
    float scaleX = (x2 - x1) / 20;

    float offY = (y1 + y2) / 2;
    float scaleY = (y2 - y1) / 20;

    float* strokeVerts = (float*)malloc(3 * MAX_CHAR_VERTS * sizeof(float));
    if (strokeVerts == 0) {
        return;
    }
    int nVerts = 0;
    int offset = 0;
    bool swap = 0;
    int flipX = 0;
    int flipY = 0;
    if (character == 0x2A || character == 0x3F || character == 0x5F) { // special characters
        character = 36 + (character == 0x3F) + 2 * (character == 0x5F);
    } else if (character >= 0x30 && character <= 0x39) {
        character = character - 0x30 + 26;
    } else {
        character -= 0x41;
        if (character >= 0x20) character -= 0x20;
        if (character < 0 || character > 25) goto DONE;
    }

    nVerts = vertCounts[character];
    offset = vertOffsets[character];
    swap = vertFlipsSwaps[character] & 0b100;
    flipX = 1 - (vertFlipsSwaps[character] & 0b010);
    flipY = 1 - ((vertFlipsSwaps[character] & 0b001) << 1);
    if (nVerts > 0) {
        for (int i = 0; i < nVerts; i++) {
            strokeVerts[i * 3] = offX + scaleX * flipX * (swap ? characterVerts[(i + offset) * 2 + 1] : characterVerts[(i + offset) * 2]);
            strokeVerts[i * 3 + 1] = offY + scaleY * flipY * (swap ? characterVerts[(i + offset) * 2] : characterVerts[(i + offset) * 2 + 1]);
            strokeVerts[i * 3 + 2] = 0;
        }

        strokeToCycle2D(strokeVerts, nVerts, buffer, samples);
    }

DONE:
    free(strokeVerts);
    return;
}