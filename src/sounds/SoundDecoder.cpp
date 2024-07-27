/****************************************************************************
 * Copyright (C) 2009-2013 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/

#include "fs/CFile.hpp"
#include <coreinit/cache.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#include <malloc.h>
#include <sounds/SoundDecoder.hpp>
#include <string.h>
#include <unistd.h>

static const uint32_t FixedPointShift = 15;
static const uint32_t FixedPointScale = 1 << FixedPointShift;

SoundDecoder::SoundDecoder() {
    file_fd = NULL;
    Init();
}

SoundDecoder::SoundDecoder(const std::string &filepath) {
    file_fd = new CFile(filepath, CFile::ReadOnly);
    Init();
}

SoundDecoder::SoundDecoder(const uint8_t *buffer, int32_t size) {
    file_fd = new CFile(buffer, size);
    Init();
}

SoundDecoder::~SoundDecoder() {
    ExitRequested = true;
    while (Decoding)
        OSSleepTicks(OSMicrosecondsToTicks(1000));

    //! lock unlock once to make sure it's really not decoding
    Lock();
    Unlock();

    if (file_fd) {
        delete file_fd;
    }
    file_fd = NULL;

    if (ResampleBuffer) {
        free(ResampleBuffer);
    }
}

int32_t SoundDecoder::Seek(int32_t pos) {
    CurPos = pos;
    return file_fd->seek(CurPos, SEEK_SET);
}

void SoundDecoder::Init() {
    SoundType       = SOUND_RAW;
    SoundBlocks     = 8;
    SoundBlockSize  = 0x4000;
    ResampleTo48kHz = false;
    CurPos          = 0;
    whichLoad       = 0;
    Loop            = false;
    EndOfFile       = false;
    Decoding        = false;
    ExitRequested   = false;
    SoundBuffer.SetBufferBlockSize(SoundBlockSize);
    SoundBuffer.Resize(SoundBlocks);
    ResampleBuffer = NULL;
    ResampleRatio  = 0;
}

int32_t SoundDecoder::Rewind() {
    CurPos    = 0;
    EndOfFile = false;
    file_fd->rewind();

    return 0;
}

int32_t SoundDecoder::Read(uint8_t *buffer, int32_t buffer_size, int32_t pos) {
    int32_t ret = file_fd->read(buffer, buffer_size);
    CurPos += ret;

    return ret;
}

void SoundDecoder::EnableUpsample(void) {
    if ((ResampleBuffer == NULL) && IsStereo() && Is16Bit() && SampleRate != 32000 && SampleRate != 48000) {
        ResampleBuffer = (uint8_t *) memalign(32, SoundBlockSize);
        ResampleRatio  = (FixedPointScale * SampleRate) / 48000;
        SoundBlockSize = (SoundBlockSize * ResampleRatio) / FixedPointScale;
        SoundBlockSize &= ~0x03;
        // set new sample rate
        SampleRate = 48000;
    }
}

void SoundDecoder::Upsample(int16_t *src, int16_t *dst, uint32_t nr_src_samples, uint32_t nr_dst_samples) {
    int32_t timer = 0;

    for (uint32_t i = 0, n = 0; i < nr_dst_samples; i += 2) {
        if ((n + 3) < nr_src_samples) {
            // simple fixed point linear interpolation
            dst[i]     = src[n] + (((src[n + 2] - src[n]) * timer) >> FixedPointShift);
            dst[i + 1] = src[n + 1] + (((src[n + 3] - src[n + 1]) * timer) >> FixedPointShift);
        } else {
            dst[i]     = src[n];
            dst[i + 1] = src[n + 1];
        }

        timer += ResampleRatio;

        if (timer >= (int32_t) FixedPointScale) {
            n += 2;
            timer -= FixedPointScale;
        }
    }
}

void SoundDecoder::Decode() {
    if (!file_fd || ExitRequested || EndOfFile) {
        return;
    }

    // check if we are not at the pre-last buffer (last buffer is playing)
    uint16_t whichPlaying = SoundBuffer.Which();
    if (((whichPlaying == 0) && (whichLoad == SoundBuffer.Size() - 2)) || ((whichPlaying == 1) && (whichLoad == SoundBuffer.Size() - 1)) || (whichLoad == (whichPlaying - 2))) {
        return;
    }

    Decoding = true;

    int32_t done       = 0;
    uint8_t *write_buf = SoundBuffer.GetBuffer(whichLoad);
    if (!write_buf) {
        ExitRequested = true;
        Decoding      = false;
        return;
    }

    if (ResampleTo48kHz && !ResampleBuffer) {
        EnableUpsample();
    }

    while (done < SoundBlockSize) {
        int32_t ret = Read(&write_buf[done], SoundBlockSize - done, Tell());

        if (ret <= 0) {
            if (Loop) {
                Rewind();
                continue;
            } else {
                EndOfFile = true;
                break;
            }
        }

        done += ret;
    }

    if (done > 0) {
        // check if we need to resample
        if (ResampleBuffer && ResampleRatio) {
            memcpy(ResampleBuffer, write_buf, done);

            int32_t src_samples  = done >> 1;
            int32_t dest_samples = (src_samples * FixedPointScale) / ResampleRatio;
            dest_samples &= ~0x01;
            Upsample((int16_t *) ResampleBuffer, (int16_t *) write_buf, src_samples, dest_samples);
            done = dest_samples << 1;
        }

        //! TODO: remove this later and add STEREO support with two voices, for now we convert to MONO
        if (IsStereo()) {
            int16_t *monoBuf = (int16_t *) write_buf;
            done             = done >> 1;

            for (int32_t i = 0; i < done; i++) {
                monoBuf[i] = monoBuf[i << 1];
            }
        }

        DCFlushRange(write_buf, done);
        SoundBuffer.SetBufferSize(whichLoad, done);
        SoundBuffer.SetBufferReady(whichLoad, true);
        if (++whichLoad >= SoundBuffer.Size()) {
            whichLoad = 0;
        }
    }

    // check if next in queue needs to be filled as well and do so
    if (!SoundBuffer.IsBufferReady(whichLoad)) {
        Decode();
    }

    Decoding = false;
}
