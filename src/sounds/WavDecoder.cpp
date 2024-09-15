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
#include "utils/utils.h"
#include <sounds/WavDecoder.hpp>
#include <cstring>


WavDecoder::WavDecoder(std::span<uint8_t> snd)
    : SoundDecoder(snd) {
    SoundType  = SOUND_WAV;
    SampleRate = 48000;
    Format     = CHANNELS_STEREO | FORMAT_PCM_16_BIT;

    if (!file_fd) {
        return;
    }

    OpenFile();
}

WavDecoder::~WavDecoder() = default;


void WavDecoder::OpenFile() {
    SWaveHdr Header;
    SWaveFmtChunk FmtChunk;
    memset(&Header, 0, sizeof(SWaveHdr));
    memset(&FmtChunk, 0, sizeof(SWaveFmtChunk));

    file_fd->read((uint8_t *) &Header, sizeof(SWaveHdr));
    file_fd->read((uint8_t *) &FmtChunk, sizeof(SWaveFmtChunk));

    if (Header.magicRIFF != 0x52494646) { // 'RIFF'
        CloseFile();
        return;
    } else if (Header.magicWAVE != 0x57415645) { // 'WAVE'
        CloseFile();
        return;
    } else if (FmtChunk.magicFMT != 0x666d7420) { // 'fmt '
        CloseFile();
        return;
    }

    DataOffset = sizeof(SWaveHdr) + le32(FmtChunk.size) + 8;
    file_fd->seek(DataOffset, SEEK_SET);
    SWaveChunk DataChunk;
    file_fd->read((uint8_t *) &DataChunk, sizeof(SWaveChunk));

    while (DataChunk.magicDATA != 0x64617461) { // 'data'
        DataOffset += 8 + le32(DataChunk.size);
        file_fd->seek(DataOffset, SEEK_SET);
        int32_t ret = file_fd->read((uint8_t *) &DataChunk, sizeof(SWaveChunk));
        if (ret <= 0) {
            CloseFile();
            return;
        }
    }

    DataOffset += 8;
    DataSize   = le32(DataChunk.size);
    Is16Bit    = (le16(FmtChunk.bps) == 16);
    SampleRate = le32(FmtChunk.freq);

    if (le16(FmtChunk.channels) == 1 && le16(FmtChunk.bps) == 8 && le16(FmtChunk.alignment) <= 1) {
        Format = CHANNELS_MONO | FORMAT_PCM_8_BIT;
    } else if (le16(FmtChunk.channels) == 1 && le16(FmtChunk.bps) == 16 && le16(FmtChunk.alignment) <= 2) {
        Format = CHANNELS_MONO | FORMAT_PCM_16_BIT;
    } else if (le16(FmtChunk.channels) == 2 && le16(FmtChunk.bps) == 8 && le16(FmtChunk.alignment) <= 2) {
        Format = CHANNELS_STEREO | FORMAT_PCM_8_BIT;
    } else if (le16(FmtChunk.channels) == 2 && le16(FmtChunk.bps) == 16 && le16(FmtChunk.alignment) <= 4) {
        Format = CHANNELS_STEREO | FORMAT_PCM_16_BIT;
    }
}

void WavDecoder::CloseFile() {
    if (file_fd) {
        file_fd.reset();
    }

    file_fd = nullptr;
}

int32_t WavDecoder::Read(uint8_t *buffer, int32_t buffer_size, int32_t pos) {
    if (!file_fd) {
        return -1;
    }

    if (CurPos >= (int32_t) DataSize) {
        return 0;
    }

    file_fd->seek(DataOffset + CurPos, SEEK_SET);

    if (buffer_size > (int32_t) DataSize - CurPos) {
        buffer_size = DataSize - CurPos;
    }

    int32_t read = file_fd->read(buffer, buffer_size);
    if (read > 0) {
        if (Is16Bit) {
            read &= ~0x0001;

            for (uint32_t i = 0; i < (uint32_t) (read / sizeof(uint16_t)); ++i) {
                ((uint16_t *) buffer)[i] = le16(((uint16_t *) buffer)[i]);
            }
        }
        CurPos += read;
    }

    return read;
}
