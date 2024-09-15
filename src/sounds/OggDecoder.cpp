/***************************************************************************
 * Copyright (C) 2010
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 *
 * for WiiXplorer 2010
 ***************************************************************************/
#include "fs/CFile.hpp"
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <malloc.h>
#include <sounds/OggDecoder.hpp>
#include <unistd.h>

static int ogg_read(void *punt, int bytes, int blocks, int *f) {
    return ((CFile *) f)->read((uint8_t *) punt, bytes * blocks);
}

static int ogg_seek(int *f, ogg_int64_t offset, int mode) {
    return ((CFile *) f)->seek((uint64_t) offset, mode);
}

static int ogg_close(int *f) {
    ((CFile *) f)->close();
    return 0;
}

static long ogg_tell(int *f) {
    return (long) ((CFile *) f)->tell();
}

static ov_callbacks callbacks = {
        (size_t(*)(void *, size_t, size_t, void *)) ogg_read,
        (int (*)(void *, ogg_int64_t, int)) ogg_seek,
        (int (*)(void *)) ogg_close,
        (long (*)(void *)) ogg_tell};



OggDecoder::OggDecoder(std::span<uint8_t> snd)
    : SoundDecoder(snd) {
    SoundType = SOUND_OGG;

    if (!file_fd) {
        return;
    }

    OpenFile();
}

OggDecoder::~OggDecoder() {
    ExitRequested = true;
    while (Decoding) {
        OSSleepTicks(OSMicrosecondsToTicks(100));
    }

    if (file_fd) {
        ov_clear(&ogg_file);
    }
}

void OggDecoder::OpenFile() {
    if (ov_open_callbacks(file_fd.get(), &ogg_file, nullptr, 0, callbacks) < 0) {
        file_fd.reset();
        return;
    }

    ogg_info = ov_info(&ogg_file, -1);
    if (!ogg_info) {
        ov_clear(&ogg_file);
        file_fd.reset();
        return;
    }

    Format     = ((ogg_info->channels == 2) ? (FORMAT_PCM_16_BIT | CHANNELS_STEREO) : (FORMAT_PCM_16_BIT | CHANNELS_MONO));
    SampleRate = ogg_info->rate;
}

int32_t OggDecoder::Rewind() {
    if (!file_fd) {
        return -1;
    }

    int32_t ret = ov_time_seek(&ogg_file, 0);
    CurPos      = 0;
    EndOfFile   = false;

    return ret;
}

int32_t OggDecoder::Read(uint8_t *buffer, int32_t buffer_size, int32_t pos) {
    if (!file_fd) {
        return -1;
    }

    int32_t bitstream = 0;

    int32_t read = (int32_t) ov_read(&ogg_file, (char *) buffer, (int) buffer_size, (int *) &bitstream);

    if (read > 0) {
        CurPos += read;
    }

    return read;
}
