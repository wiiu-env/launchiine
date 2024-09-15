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
#ifndef SOUND_DECODER_HPP
#define SOUND_DECODER_HPP

#include <mutex>
#include <sounds/BufferCircle.hpp>
#include <string>
#include <span>

class CFile;

class SoundDecoder {
public:
    SoundDecoder();

    SoundDecoder(const std::string &filepath);

    SoundDecoder(std::span<uint8_t> snd);

    virtual ~SoundDecoder();

    virtual void Lock() {
        mutex.lock();
    }

    virtual void Unlock() {
        mutex.unlock();
    }

    virtual int32_t Read(uint8_t *buffer, int32_t buffer_size, int32_t pos);

    virtual int32_t Tell() {
        return CurPos;
    }

    virtual int32_t Seek(int32_t pos);

    virtual int32_t Rewind();

    virtual uint16_t GetFormat() {
        return Format;
    }

    virtual uint16_t GetSampleRate() {
        return SampleRate;
    }

    virtual void Decode();

    virtual bool IsBufferReady() {
        return SoundBuffer.IsBufferReady();
    }

    virtual uint8_t *GetBuffer() {
        return SoundBuffer.GetBuffer();
    }

    virtual uint32_t GetBufferSize() {
        return SoundBuffer.GetBufferSize();
    }

    virtual void LoadNext() {
        SoundBuffer.LoadNext();
    }

    virtual bool IsEOF() {
        return EndOfFile;
    }

    virtual void SetLoop(bool l) {
        Loop      = l;
        EndOfFile = false;
    }

    virtual uint8_t GetSoundType() {
        return SoundType;
    }

    virtual void ClearBuffer() {
        SoundBuffer.ClearBuffer();
        whichLoad = 0;
    }

    virtual bool IsStereo() {
        return (GetFormat() & CHANNELS_STEREO) != 0;
    }

    virtual bool Is16Bit() {
        return ((GetFormat() & 0xFF) == FORMAT_PCM_16_BIT);
    }

    virtual bool IsDecoding() {
        return Decoding;
    }

    void EnableUpsample();

    enum SoundFormats {
        FORMAT_PCM_16_BIT = 0x0A,
        FORMAT_PCM_8_BIT  = 0x19,
    };
    enum SoundChannels {
        CHANNELS_MONO   = 0x100,
        CHANNELS_STEREO = 0x200
    };

    enum SoundType {
        SOUND_RAW = 0,
        SOUND_MP3,
        SOUND_OGG,
        SOUND_WAV
    };

protected:
    void Init();

    void Upsample(int16_t *src, int16_t *dst, uint32_t nr_src_samples, uint32_t nr_dst_samples);

    std::unique_ptr<CFile> file_fd;
    BufferCircle SoundBuffer;
    uint8_t SoundType{};
    uint16_t whichLoad{};
    uint16_t SoundBlocks{};
    int32_t SoundBlockSize{};
    int32_t CurPos{};
    bool ResampleTo48kHz{};
    bool Loop{};
    bool EndOfFile{};
    bool Decoding{};
    bool ExitRequested{};
    uint16_t Format{};
    uint16_t SampleRate{};
    uint8_t *ResampleBuffer{};
    uint32_t ResampleRatio{};
    std::recursive_mutex mutex;
};


#endif
