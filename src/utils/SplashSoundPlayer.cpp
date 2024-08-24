#include "SplashSoundPlayer.h"
#include "logger.h"
#include "utils.h"
#include "utils.c"
#include <coreinit/transition.h>
#include <cstring>
#include <sndcore2/core.h>
#include <sndcore2/device.h>
#include <span>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <whb/log.h>
#include <fstream>

SplashSoundPlayer::SplashSoundPlayer(std::string_view meta_dir) {
    std::string bootSound = std::string(meta_dir).append("/bootSound.btsnd");

    if (!LoadFileIntoBuffer(bootSound, mBuffer)) {
        mBuffer.clear();
        return;
    }
    if (mBuffer.size() < 8) {
        return;
    }

    auto target = ((uint32_t *) mBuffer.data())[0];
    if (target <= 2) {
        mOutputTarget = static_cast<TransitionAudioTarget>(target);
    }

    mLoopPoint = ((uint32_t *) mBuffer.data())[1];
}

void SplashSoundPlayer::Play() {
    if (mBuffer.empty() || mBuffer.size() < 8) {
        DEBUG_FUNCTION_LINE_WRITE("mBuffer is empty or too small");
        return;
    }
    AXTransitionAudioBuffer transitionAudioBuffer = {};

    AXInit();
    AXDeviceMode tvMode;
    AXDeviceMode drcMode;
    AXGetDeviceMode(0, &tvMode);
    AXGetDeviceMode(1, &drcMode);
    AXQuit();

    WHBLogPrintf("TV mode before transition = %d", tvMode);
    transitionAudioBuffer.tv.mode = tvMode;

    WHBLogPrintf("DRC mode before transition = %d", drcMode);
    transitionAudioBuffer.drc.mode = drcMode;

    transitionAudioBuffer.unk1 = 1;
    transitionAudioBuffer.unk2 = 0;

    transitionAudioBuffer.tv.unk1 = 0.99987207655f;
    transitionAudioBuffer.tv.unk2 = 600;

    transitionAudioBuffer.drc.unk1 = transitionAudioBuffer.tv.unk1;
    transitionAudioBuffer.tv.unk2  = transitionAudioBuffer.tv.unk2;

    switch (mOutputTarget) {
        case TV_ONLY:
            transitionAudioBuffer.tv.enabled  = true;
            transitionAudioBuffer.drc.enabled = false;
            break;
        case DRC_ONLY:
            transitionAudioBuffer.tv.enabled  = false;
            transitionAudioBuffer.drc.enabled = true;
            break;
        case BOTH:
            transitionAudioBuffer.tv.enabled  = true;
            transitionAudioBuffer.drc.enabled = true;
            break;
    }

    void *audioBuffer       = (void *) nullptr;
    uint32_t audioBufferLen = 0;
    auto res                = __OSGetTransitionAudioBuffer(&audioBuffer, &audioBufferLen);
    if (res == 0) {
        DEBUG_FUNCTION_LINE_WRITE("Could not get access to audio buffer from foreground bucket");
        return;
    }

    std::span<uint8_t> audioBufferIn(mBuffer.data() + 8, mBuffer.size() - 8);

    DEBUG_FUNCTION_LINE("Got audio buffer from foreground bucket @ %8.8x len = %d", audioBuffer, audioBufferLen);
    if (audioBufferLen < mBuffer.size()) {
        DEBUG_FUNCTION_LINE_WRITE("buffer not big enough");
        return;
    }

    memcpy(audioBuffer, audioBufferIn.data(), audioBufferIn.size());
    __OSSetTransitionAudioSize(audioBufferIn.size());

    transitionAudioBuffer.length         = audioBufferIn.size();
    transitionAudioBuffer.loopPoint      = mLoopPoint;
    transitionAudioBuffer.audioBuffer    = audioBuffer;
    transitionAudioBuffer.audioBufferLen = audioBufferLen;
    AXSetUpTransitionAudio((AXTransitionAudioBuffer *) &transitionAudioBuffer);
    AXStartTransitionAudio();
}
