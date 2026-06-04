/**
 * Copyright (c) 2015-2024	, Martin Roth (mhroth@gmail.com)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "tinywav.h"

#include <malloc.h>

#define TW_ALLOC(type, var, sz) type* var = (type*) malloc((sz)*sizeof(type))

#define TW_DEALLOC(x) free(x)

 // MARK: private functions

 /** @returns true if the chunk of 4 characters matches the supplied string */
static bool chunkIDMatches(char chunk[4], const char* chunkName)
{
    for (int i = 0; i < 4; ++i) {
        if (chunk[i] != chunkName[i]) {
            return false;
        }
    }
    return true;
}

// MARK: public functions

unsigned int tinywav_open_read(TinyWav* tw, const char* path, TinyWavChannelFormat chanFmt) {

    if (tw == NULL || path == NULL) {
        return 0;
    }

    errno_t err = fopen_s(&tw->f, path, "rb");
    if (err != 0) { tw->f = NULL; }

    if (tw->f == NULL) {
        return 0;
    }

    // Parse WAV header
    /** @note: We do this byte-by-byte to avoid dependencies (htonl() et al.) and because struct padding depends on
     *  specific compiler implementation ('slurping' directly into the header struct is therefore dangerous).
     *  The RIFF format specifies little-endian order for the data stream. */

     // RIFF Chunk, WAVE Subchunk
    size_t elementCount = fread(tw->h.ChunkID, sizeof(char), 4, tw->f);
    elementCount += fread(&tw->h.ChunkSize, sizeof(uint32_t), 1, tw->f);
    elementCount += fread(tw->h.Format, sizeof(char), 4, tw->f);

    if (elementCount < 9 || !chunkIDMatches(tw->h.ChunkID, "RIFF") || !chunkIDMatches(tw->h.Format, "WAVE")) {
        tinywav_close_read(tw);
        return 0;
    }

    // Go through subchunks until we find 'fmt '  (There are sometimes JUNK or other chunks before 'fmt ')
    while (fread(tw->h.Subchunk1ID, sizeof(char), 4, tw->f) == 4) {
        fread(&tw->h.Subchunk1Size, sizeof(uint32_t), 1, tw->f);
        if (chunkIDMatches(tw->h.Subchunk1ID, "fmt ")) {
            break;
        }
        else {
            fseek(tw->f, tw->h.Subchunk1Size, SEEK_CUR); // skip this subchunk
        }
    }

    // fmt Subchunk
    elementCount = fread(&tw->h.AudioFormat, sizeof(uint16_t), 1, tw->f);
    elementCount += fread(&tw->h.NumChannels, sizeof(uint16_t), 1, tw->f);
    elementCount += fread(&tw->h.SampleRate, sizeof(uint32_t), 1, tw->f);
    elementCount += fread(&tw->h.ByteRate, sizeof(uint32_t), 1, tw->f);
    elementCount += fread(&tw->h.BlockAlign, sizeof(uint16_t), 1, tw->f);
    elementCount += fread(&tw->h.BitsPerSample, sizeof(uint16_t), 1, tw->f);
    if (elementCount != 6) {
        tinywav_close_read(tw);
        return 0;
    }

    // Sanity checks
    if (tw->h.NumChannels < 1 || tw->h.NumChannels > 128) { // relevant because
        tinywav_close_read(tw);
        return 0;
    }
    if (tw->h.SampleRate < 1) {
        tinywav_close_read(tw);
        return 0;
    }

    // skip over any other chunks before the "data" chunk (e.g. JUNK, INFO, bext, ...)
    while (fread(tw->h.Subchunk2ID, sizeof(char), 4, tw->f) == 4) {
        fread(&tw->h.Subchunk2Size, sizeof(uint32_t), 1, tw->f);
        if (chunkIDMatches(tw->h.Subchunk2ID, "data")) {
            break;
        }
        else {
            fseek(tw->f, tw->h.Subchunk2Size, SEEK_CUR); // skip this subchunk
        }
    }

    tw->numChannels = tw->h.NumChannels;
    tw->chanFmt = chanFmt;

    if (tw->h.BitsPerSample != 16 || tw->h.AudioFormat != 1) {
        printf("Loaded file uses an incompatible format! Only Microsoft 16-bit signed PCM is supported. (ffmpeg uses Wave Extended, which is also not supported, but Audacity works correctly)\n");
        tinywav_close_read(tw);
        return 0;
    }

    // NOTE: previous sanity checks ensure div by zero is not possible here
    tw->numFramesInHeader = tw->h.Subchunk2Size / (tw->numChannels * 2);
    tw->totalFramesReadWritten = 0;

    return tw->h.SampleRate;
}

int tinywav_read_f(TinyWav* tw, void* data, int len) {

    if (tw == NULL || data == NULL || len < 0 || len > tw->numFramesInHeader) {
        return -1;
    }

    if (tw->totalFramesReadWritten * tw->h.BlockAlign >= tw->h.Subchunk2Size) {
        // We are past the 'data' subchunk (size as declared in header).
        // Sometimes there are additionl chunks *after* -- ignore these.
        return 0; // there's nothing more to read, not an error.
    }

    int ret = 0;

    TW_ALLOC(int16_t, interleaved_data, tw->numChannels * len);
    if (!interleaved_data) return -1;
    size_t samples_read = fread(interleaved_data, sizeof(int16_t), tw->numChannels * len, tw->f);
    uint32_t frames_read_u32 = (uint32_t)(samples_read / tw->numChannels);
    tw->totalFramesReadWritten += frames_read_u32;
    int frames_read = (int)frames_read_u32;
    for (int pos = 0; pos < tw->numChannels * frames_read; pos++) {
        ((short*)data)[pos] = interleaved_data[pos];
    }
    ret = frames_read;
    TW_DEALLOC(interleaved_data);

    return ret;
}

void tinywav_close_read(TinyWav* tw) {
    if (tw->f == NULL) {
        return; // fclose(NULL) is undefined behaviour
    }

    fclose(tw->f);
    tw->f = NULL;
}