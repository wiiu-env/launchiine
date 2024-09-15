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
#include <fs/CFile.hpp>
#include <malloc.h>
#include <sndcore2/core.h>
#include <sounds/Mp3Decoder.hpp>
#include <sounds/OggDecoder.hpp>
#include <sounds/SoundHandler.hpp>
#include <sounds/WavDecoder.hpp>
#include <span>
#include <unistd.h>

SoundHandler *SoundHandler::handlerInstance = NULL;

SoundHandler::SoundHandler()
    : CThread(CThread::eAttributeAffCore1 | CThread::eAttributePinnedAff, 0, 0x8000) {
    Decoding      = false;
    ExitRequested = false;
    for (uint32_t i = 0; i < MAX_DECODERS; ++i) {
        DecoderList[i].reset();
        voiceList[i].reset();
    }

    resumeThread();

    //! wait for initialization
    while (!isThreadSuspended()) {
        OSSleepTicks(OSMicrosecondsToTicks(1000));
    }
}

SoundHandler::~SoundHandler() {
    ExitRequested = true;
    ThreadSignal();

    ClearDecoderList();
}

void SoundHandler::AddDecoder(int32_t voice, const std::span<uint8_t> snd) {
    if (voice < 0 || voice >= MAX_DECODERS) {
        return;
    }

    if (DecoderList[voice] != nullptr) {
        RemoveDecoder(voice);
    }

    DecoderList[voice] = GetSoundDecoder(snd);
}

void SoundHandler::RemoveDecoder(int32_t voice) {
    if (voice < 0 || voice >= MAX_DECODERS) {
        return;
    }

    if (DecoderList[voice] != nullptr) {
        if (voiceList[voice] && voiceList[voice]->getState() != Voice::STATE_STOPPED) {
            if (voiceList[voice]->getState() != Voice::STATE_STOP) {
                voiceList[voice]->setState(Voice::STATE_STOP);
            }

            // it shouldn't take longer than 3 ms actually but we wait up to 20
            // on application quit the AX frame callback is not called anymore
            // therefore this would end in endless loop if no timeout is defined
            int timeOut = 20;
            while (--timeOut && (voiceList[voice]->getState() != Voice::STATE_STOPPED))
                OSSleepTicks(OSMicrosecondsToTicks(1000));
        }
        DecoderList[voice].reset();
    }
}

void SoundHandler::ClearDecoderList() {
    for (uint32_t i = 0; i < MAX_DECODERS; ++i) {
        RemoveDecoder(i);
    }
}

static inline bool CheckMP3Signature(const uint8_t *buffer) {
    const uint8_t MP3_Magic[][3] = {
            {'I', 'D', '3'}, //'ID3'
            {0xff, 0xfe},    //'MPEG ADTS, layer III, v1.0 [protected]', 'mp3', 'audio/mpeg'),
            {0xff, 0xff},    //'MPEG ADTS, layer III, v1.0', 'mp3', 'audio/mpeg'),
            {0xff, 0xfa},    //'MPEG ADTS, layer III, v1.0 [protected]', 'mp3', 'audio/mpeg'),
            {0xff, 0xfb},    //'MPEG ADTS, layer III, v1.0', 'mp3', 'audio/mpeg'),
            {0xff, 0xf2},    //'MPEG ADTS, layer III, v2.0 [protected]', 'mp3', 'audio/mpeg'),
            {0xff, 0xf3},    //'MPEG ADTS, layer III, v2.0', 'mp3', 'audio/mpeg'),
            {0xff, 0xf4},    //'MPEG ADTS, layer III, v2.0 [protected]', 'mp3', 'audio/mpeg'),
            {0xff, 0xf5},    //'MPEG ADTS, layer III, v2.0', 'mp3', 'audio/mpeg'),
            {0xff, 0xf6},    //'MPEG ADTS, layer III, v2.0 [protected]', 'mp3', 'audio/mpeg'),
            {0xff, 0xf7},    //'MPEG ADTS, layer III, v2.0', 'mp3', 'audio/mpeg'),
            {0xff, 0xe2},    //'MPEG ADTS, layer III, v2.5 [protected]', 'mp3', 'audio/mpeg'),
            {0xff, 0xe3},    //'MPEG ADTS, layer III, v2.5', 'mp3', 'audio/mpeg'),
    };

    if (buffer[0] == MP3_Magic[0][0] && buffer[1] == MP3_Magic[0][1] &&
        buffer[2] == MP3_Magic[0][2]) {
        return true;
    }

    for (int32_t i = 1; i < 13; i++) {
        if (buffer[0] == MP3_Magic[i][0] && buffer[1] == MP3_Magic[i][1]) {
            return true;
        }
    }

    return false;
}

std::unique_ptr<SoundDecoder> SoundHandler::GetSoundDecoder(std::span<uint8_t> snd) {
    const uint8_t *check = snd.data();
    int32_t counter      = 0;

    while (check[0] == 0 && counter < snd.size()) {
        check++;
        counter++;
    }

    if (counter >= snd.size()) {
        return nullptr;
    }

    auto *magic = (uint32_t *) check;

    if (magic[0] == 0x4f676753) { // 'OggS'
        return std::make_unique<OggDecoder>(snd);
    } else if (magic[0] == 0x52494646) { // 'RIFF'
        return std::make_unique<WavDecoder>(snd);
    } else if (CheckMP3Signature(check)) {
        return std::make_unique<Mp3Decoder>(snd);
    }

    return std::make_unique<SoundDecoder>(snd);
}

void SoundHandler::executeThread() {
    /*// v2 sound lib can not properly end transition audio on old firmwares
    if (OS_FIRMWARE >= 400 && OS_FIRMWARE <= 410)
    {
        ProperlyEndTransitionAudio();
    }*/


    //! initialize 48 kHz renderer
    AXInitParams params;
    memset(&params, 0, sizeof(params));
    params.renderer = AX_INIT_RENDERER_48KHZ;

    // TODO: handle support for 3.1.0 with dynamic libs instead of static linking it
    //if(AXInitWithParams != 0)
    AXInitWithParams(&params);
    //else
    //    AXInit();

    // The problem with last voice on 500 was caused by it having priority 0
    // We would need to change this priority distribution if for some reason
    // we would need MAX_DECODERS > Voice::PRIO_MAX
    for (uint32_t i = 0; i < MAX_DECODERS; ++i) {
        int32_t priority = (MAX_DECODERS - i) * Voice::PRIO_MAX / MAX_DECODERS;
        voiceList[i]     = std::make_unique<Voice>(priority); // allocate voice 0 with highest priority
    }

    AXRegisterAppFrameCallback(SoundHandler::axFrameCallback);


    uint16_t i = 0;
    while (!ExitRequested) {
        suspendThread();

        for (i = 0; i < MAX_DECODERS; ++i) {
            if (DecoderList[i] == nullptr) {
                continue;
            }

            Decoding = true;
            if (DecoderList[i]) {
                DecoderList[i]->Lock();
            }
            if (DecoderList[i]) {
                DecoderList[i]->Decode();
            }
            if (DecoderList[i]) {
                DecoderList[i]->Unlock();
            }
        }
        Decoding = false;
    }

    for (auto &voice : voiceList) {
        voice->stop();
    }

    AXRegisterAppFrameCallback(nullptr);
    AXQuit();

    for (auto &voice : voiceList) {
        voice.reset();
    }
}

void SoundHandler::axFrameCallback() {
    for (uint32_t i = 0; i < MAX_DECODERS; i++) {
        Voice *voice = handlerInstance->getVoice(i);

        switch (voice->getState()) {
            default:
            case Voice::STATE_STOPPED:
                break;

            case Voice::STATE_START: {
                SoundDecoder *decoder = handlerInstance->getDecoder(i);
                decoder->Lock();
                if (decoder->IsBufferReady()) {
                    const uint8_t *buffer     = decoder->GetBuffer();
                    const uint32_t bufferSize = decoder->GetBufferSize();
                    decoder->LoadNext();

                    const uint8_t *nextBuffer = nullptr;
                    uint32_t nextBufferSize   = 0;

                    if (decoder->IsBufferReady()) {
                        nextBuffer     = decoder->GetBuffer();
                        nextBufferSize = decoder->GetBufferSize();
                        decoder->LoadNext();
                    }

                    voice->play(buffer, bufferSize, nextBuffer, nextBufferSize, decoder->GetFormat() & 0xff, decoder->GetSampleRate());

                    handlerInstance->ThreadSignal();

                    voice->setState(Voice::STATE_PLAYING);
                }
                decoder->Unlock();
                break;
            }
            case Voice::STATE_PLAYING:
                if (voice->getInternState() == 1) {
                    if (voice->isBufferSwitched()) {
                        SoundDecoder *decoder = handlerInstance->getDecoder(i);
                        decoder->Lock();
                        if (decoder->IsBufferReady()) {
                            voice->setNextBuffer(decoder->GetBuffer(), decoder->GetBufferSize());
                            decoder->LoadNext();
                            handlerInstance->ThreadSignal();
                        } else if (decoder->IsEOF()) {
                            voice->setState(Voice::STATE_STOP);
                        }
                        decoder->Unlock();
                    }
                } else {
                    voice->setState(Voice::STATE_STOPPED);
                }
                break;
            case Voice::STATE_STOP:
                if (voice->getInternState() != 0) {
                    voice->stop();
                }
                voice->setState(Voice::STATE_STOPPED);
                break;
        }
    }
}
