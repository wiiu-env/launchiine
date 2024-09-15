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
#include <gui/GuiSound.h>
#include <sounds/SoundHandler.hpp>
#include <string>

GuiSound::GuiSound(std::vector<uint8_t>&& snd) {
    voice = -1;
    internalBuffer = std::move(snd);
}

GuiSound::~GuiSound() {
    if (voice >= 0) {
        SoundHandler::instance()->RemoveDecoder(voice);
    }
}

bool GuiSound::Load() {
    if (voice >= 0) {
        SoundHandler::instance()->RemoveDecoder(voice);
        voice = -1;
    }

    if (!internalBuffer.empty()) {
        return false;
    }

    //! find next free decoder
    for (int32_t i = 0; i < MAX_DECODERS; i++) {
        SoundDecoder *decoder = SoundHandler::instance()->getDecoder(i);
        if (decoder == nullptr) {
            SoundHandler::instance()->AddDecoder(i, internalBuffer);
            decoder = SoundHandler::instance()->getDecoder(i);
            if (decoder) {
                voice = i;
                SoundHandler::instance()->ThreadSignal();
            }
            break;
        }
    }

    if (voice < 0) {
        return false;
    }

    return true;
}

void GuiSound::Play() {
    Stop();

    Voice *v = SoundHandler::instance()->getVoice(voice);
    if (v) {
        v->setState(Voice::STATE_START);
    }
}

void GuiSound::Stop() {
    Voice *v = SoundHandler::instance()->getVoice(voice);
    if (v) {
        if ((v->getState() != Voice::STATE_STOP) && (v->getState() != Voice::STATE_STOPPED)) {
            v->setState(Voice::STATE_STOP);
        }

        while (v->getState() != Voice::STATE_STOPPED)
            OSSleepTicks(OSMicrosecondsToTicks(1000));
    }

    SoundDecoder *decoder = SoundHandler::instance()->getDecoder(voice);
    if (decoder) {
        decoder->Lock();
        decoder->Rewind();
        decoder->ClearBuffer();
        SoundHandler::instance()->ThreadSignal();
        decoder->Unlock();
    }
}

void GuiSound::Pause() {
    if (!IsPlaying()) {
        return;
    }

    Voice *v = SoundHandler::instance()->getVoice(voice);
    if (v) {
        v->setState(Voice::STATE_STOP);
    }
}

void GuiSound::Resume() {
    if (IsPlaying()) {
        return;
    }

    Voice *v = SoundHandler::instance()->getVoice(voice);
    if (v) {
        v->setState(Voice::STATE_START);
    }
}

bool GuiSound::IsPlaying() const {
    Voice *v = SoundHandler::instance()->getVoice(voice);
    if (v) {
        return v->getState() == Voice::STATE_PLAYING;
    }
    return false;
}

void GuiSound::SetVolume(uint32_t vol) {
    if (vol > 100) {
        vol = 100;
    }

    uint32_t volumeConv = ((0x8000 * vol) / 100) << 16;

    Voice *v = SoundHandler::instance()->getVoice(voice);
    if (v) {
        v->setVolume(volumeConv);
    }
}

void GuiSound::SetLoop(bool l) {
    SoundDecoder *decoder = SoundHandler::instance()->getDecoder(voice);
    if (decoder) {
        decoder->SetLoop(l);
    }
}

void GuiSound::Rewind() {
    Stop();
}
