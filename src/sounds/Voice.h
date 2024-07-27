/****************************************************************************
 * Copyright (C) 2015 Dimok
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
#ifndef _AXSOUND_H_
#define _AXSOUND_H_

#include <sndcore2/core.h>
#include <sndcore2/voice.h>

class Voice {
public:
    enum VoicePriorities {
        PRIO_MIN = 1,
        PRIO_MAX = 31
    };

    enum VoiceStates {
        STATE_STOPPED,
        STATE_START,
        STATE_PLAYING,
        STATE_STOP,
    };

    Voice(int32_t prio)
        : state(STATE_STOPPED) {
        lastLoopCounter = 0;
        nextBufferSize  = 0;

        voice = AXAcquireVoice(prio, 0, 0);
        if (voice) {
            AXVoiceBegin(voice);
            AXSetVoiceType(voice, 0);
            setVolume(0x80000000);


            AXVoiceDeviceMixData mix[6];
            memset(mix, 0, sizeof(mix));
            mix[0].bus[0].volume = 0x8000;
            mix[0].bus[0].delta  = 0;
            mix[1].bus[0].volume = 0x8000;
            mix[1].bus[0].delta  = 0;

            AXSetVoiceDeviceMix(voice, 0, 0, mix);
            AXSetVoiceDeviceMix(voice, 1, 0, mix);

            AXVoiceEnd(voice);
        }
    }

    ~Voice() {
        if (voice) {
            AXFreeVoice(voice);
        }
    }

    void play(const uint8_t *buffer, uint32_t bufferSize, const uint8_t *nextBuffer, uint32_t nextBufSize, uint16_t format, uint32_t sampleRate) {
        if (!voice) {
            return;
        }

        memset(&voiceBuffer, 0, sizeof(voiceBuffer));

        voiceBuffer.data           = buffer;
        voiceBuffer.dataType       = format;
        voiceBuffer.loopingEnabled = (nextBuffer == NULL) ? 0 : 1;
        voiceBuffer.currentOffset  = 0;
        voiceBuffer.endOffset      = (bufferSize >> 1) - 1;
        voiceBuffer.loopOffset     = ((nextBuffer - buffer) >> 1);
        nextBufferSize             = nextBufSize;

        // TODO: handle support for 3.1.0 with dynamic libs instead of static linking it
        //uint32_t samplesPerSec = (AXGetInputSamplesPerSec != 0) ? AXGetInputSamplesPerSec() : 32000;
        uint32_t samplesPerSec = AXGetInputSamplesPerSec();

        memset(&ratioBits, 0, sizeof(ratioBits));
        ratioBits.ratio = (uint32_t) (0x00010000 * ((float) sampleRate / (float) samplesPerSec));

        AXSetVoiceOffsets(voice, &voiceBuffer);
        AXSetVoiceSrc(voice, &ratioBits);
        AXSetVoiceSrcType(voice, 1);
        AXSetVoiceState(voice, 1);
    }

    void stop() {
        if (voice) {
            AXSetVoiceState(voice, 0);
        }
    }

    void setVolume(uint32_t vol) {
        if (voice) {
            AXVoiceVeData data;
            data.volume = vol >> 16;
            data.delta  = vol & 0xFFFF;
            AXSetVoiceVe(voice, &data);
        }
    }


    void setNextBuffer(const uint8_t *buffer, uint32_t bufferSize) {
        voiceBuffer.loopOffset = ((buffer - (const uint8_t *) voiceBuffer.data) >> 1);
        nextBufferSize         = bufferSize;

        AXSetVoiceLoopOffset(voice, voiceBuffer.loopOffset);
    }

    bool isBufferSwitched() {
        uint32_t loopCounter = AXGetVoiceLoopCount(voice);
        if (lastLoopCounter != loopCounter) {
            lastLoopCounter = loopCounter;
            AXSetVoiceEndOffset(voice, voiceBuffer.loopOffset + (nextBufferSize >> 1) - 1);
            return true;
        }
        return false;
    }

    uint32_t getInternState() const {
        if (voice) {
            return ((uint32_t *) voice)[1];
        }
        return 0;
    }

    uint32_t getState() const {
        return state;
    }

    void setState(uint32_t s) {
        state = s;
    }

    void *getVoice() const {
        return voice;
    }

private:
    AXVoice *voice;
    AXVoiceSrc ratioBits;
    AXVoiceOffsets voiceBuffer;
    uint32_t state;
    uint32_t nextBufferSize;
    uint32_t lastLoopCounter;
};

#endif // _AXSOUND_H_
