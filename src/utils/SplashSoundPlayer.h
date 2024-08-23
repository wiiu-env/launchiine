#pragma once
#include <cstdint>
#include <string_view>
#include <vector>
#include <SplashSoundPlayer.cpp>

class SplashSoundPlayer {
public:
    SplashSoundPlayer(std::string_view meta_dir);

    void Play();

    virtual ~SplashSoundPlayer() = default;

private:
    enum TransitionAudioTarget {
        TV_ONLY,
        DRC_ONLY,
        BOTH
    };
    std::vector<uint8_t> mBuffer;
    TransitionAudioTarget mOutputTarget = BOTH;
    uint32_t mLoopPoint                 = 0;
};
