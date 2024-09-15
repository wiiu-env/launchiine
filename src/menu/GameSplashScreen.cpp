#include "GameSplashScreen.h"
#include "fs/FSUtils.h"
#include "utils/logger.h"
#include "utils/utils.h"
#include <memory>
#include <nn/acp/title.h>

GameSplashScreen::GameSplashScreen(int32_t w, int32_t h, gameInfo *info, bool onTV) : GuiFrame(w, h),
                                                                                      bgImageColor(w, h, (GX2Color){0, 0, 0, 0}) {
    bgImageColor.setImageColor((GX2Color){79, 153, 239, 255}, 0);
    bgImageColor.setImageColor((GX2Color){79, 153, 239, 255}, 1);
    bgImageColor.setImageColor((GX2Color){59, 159, 223, 255}, 2);
    bgImageColor.setImageColor((GX2Color){59, 159, 223, 255}, 3);
    append(&bgImageColor);
    this->onTV = onTV;
    this->info = info;

    this->effectFinished.connect(this, &GameSplashScreen::OnSplashScreenFadeInDone);


    char metaDir[256] = {};
    auto res          = ACPGetTitleMetaDir(info->titleId, metaDir, sizeof(metaDir) - 1);
    if (res != ACP_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE_WARN("Failed to find assets");
        return;
    }

    std::string filepath = onTV ? std::string(metaDir).append("/bootTvTex.tga") : std::string(metaDir).append("/bootDrcTex.tga");

    std::vector<uint8_t> buffer;
    if (Utils::LoadFileIntoBuffer(filepath, buffer)) {
        splashScreenData = std::make_unique<GuiImageData>(buffer, GX2_TEX_CLAMP_MODE_MIRROR);
        if (splashScreenData) {
            bgImageColor.setImageData(std::move(splashScreenData));
            bgImageColor.setScale(((float) h) / splashScreenData->getHeight());
        }

        //! free original image buffer which is converted to texture now and not needed anymore
    }
}

void GameSplashScreen::OnSplashScreenFadeInDone(GuiElement *element) {
    // we need to wait one more frame because the effects get calculated before drawing.
    launchGame = true;
}

void GameSplashScreen::draw(CVideo *v) {
    GuiFrame::draw(v);
    bool triggerLaunch = onTV; // Only the trigger the launch when calling for the TV.
    if (launchGame && frameCounter++ > 1) {
        launchGame = false;
        gameGameSplashScreenFinished(this, info, triggerLaunch);
    }
}

GameSplashScreen::~GameSplashScreen() = default;
