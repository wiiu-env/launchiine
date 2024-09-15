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
#ifndef SOUNDHANDLER_H_
#define SOUNDHANDLER_H_

#include <vector>

#include <array>
#include <sndcore2/voice.h>
#include <sounds/SoundDecoder.hpp>
#include <sounds/Voice.h>
#include <span>
#include <system/CThread.h>

#define MAX_DECODERS 16 // can be increased up to 96

class SoundHandler : public CThread {
public:
    static SoundHandler *instance() {
        if (!handlerInstance) {
            handlerInstance = new SoundHandler();
        }
        return handlerInstance;
    }

    static void DestroyInstance() {
        delete handlerInstance;
        handlerInstance = nullptr;
    }

    void AddDecoder(int32_t voice, std::span<uint8_t> snd);

    void RemoveDecoder(int32_t voice);

    SoundDecoder *getDecoder(int32_t i) {
        return ((i < 0 || i >= MAX_DECODERS) ? nullptr : DecoderList[i].get());
    };

    Voice* getVoice(int32_t i) {
        return ((i < 0 || i >= MAX_DECODERS) ? nullptr : voiceList[i].get());
    };

    void ThreadSignal() {
        resumeThread();
    };

    [[nodiscard]] bool IsDecoding() const {
        return Decoding;
    };

protected:
    SoundHandler();

    ~SoundHandler() override;

    static void axFrameCallback();

    void executeThread() override;

    void ClearDecoderList();

    std::unique_ptr<SoundDecoder> GetSoundDecoder(std::span<uint8_t> snd);

    static SoundHandler *handlerInstance;

    bool Decoding;
    bool ExitRequested;

    std::array<std::unique_ptr<Voice>, MAX_DECODERS> voiceList;
    std::array<std::unique_ptr<SoundDecoder>, MAX_DECODERS> DecoderList;
};

#endif
