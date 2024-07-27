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

#include <sounds/SoundDecoder.hpp>
#include <sounds/Voice.h>
#include <system/CThread.h>
#include <sndcore2/voice.h>

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
        handlerInstance = NULL;
    }

    void AddDecoder(int32_t voice, const char *filepath);

    void AddDecoder(int32_t voice, const uint8_t *snd, int32_t len);

    void RemoveDecoder(int32_t voice);

    SoundDecoder *getDecoder(int32_t i) {
        return ((i < 0 || i >= MAX_DECODERS) ? NULL : DecoderList[i]);
    };

    Voice *getVoice(int32_t i) {
        return ((i < 0 || i >= MAX_DECODERS) ? NULL : voiceList[i]);
    };

    void ThreadSignal() {
        resumeThread();
    };

    bool IsDecoding() {
        return Decoding;
    };

protected:
    SoundHandler();

    ~SoundHandler();

    static void axFrameCallback(void);

    void executeThread(void);

    void ClearDecoderList();

    SoundDecoder *GetSoundDecoder(const char *filepath);

    SoundDecoder *GetSoundDecoder(const uint8_t *sound, int32_t length);

    static SoundHandler *handlerInstance;

    bool Decoding;
    bool ExitRequested;

    Voice *voiceList[MAX_DECODERS];
    SoundDecoder *DecoderList[MAX_DECODERS];
};

#endif
