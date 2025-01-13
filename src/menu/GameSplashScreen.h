#pragma once

#include "game/GameList.h"
#include <gui/GuiFrame.h>
#include <gui/GuiImage.h>

class GameSplashScreen : public GuiFrame, public sigslot::has_slots<> {
public:
    GameSplashScreen(int32_t w, int32_t h, gameInfo *info, bool onTV);

    virtual ~GameSplashScreen();

    void OnSplashScreenFadeInDone(GuiElement *element);

    virtual void draw(CVideo *v);

    sigslot::signal3<GuiElement *, gameInfo *, bool> gameGameSplashScreenFinished;

private:
    GuiImage bgImageColor;
    GuiImageData *splashScreenData = nullptr;
    gameInfo *info                 = nullptr;
    bool launchGame                = false;
    uint32_t frameCounter          = 0;
    bool onTV                      = false;
};
