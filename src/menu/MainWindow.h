/****************************************************************************
 * Copyright (C) 2015 Dimok
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include "KeyboardHelper.h"
#include "MainDrcButtonsFrame.h"
#include "game/GameList.h"
#include "gui/GuiTitleBrowser.h"
#include <gui/Gui.h>
#include <queue>
#include <vector>

class CVideo;

class MainWindow : public sigslot::has_slots<> {
public:
    MainWindow(int32_t w, int32_t h);

    virtual ~MainWindow();

    void appendTv(GuiElement *e) {
        if (!e)
            return;

        removeTv(e);
        tvElements.push_back(e);
    }

    void appendDrc(GuiElement *e) {
        if (!e)
            return;

        removeDrc(e);
        drcElements.push_back(e);
    }

    void append(GuiElement *e) {
        appendTv(e);
        appendDrc(e);
    }

    void insertTv(uint32_t pos, GuiElement *e) {
        if (!e)
            return;

        removeTv(e);
        tvElements.insert(tvElements.begin() + pos, e);
    }

    void insertDrc(uint32_t pos, GuiElement *e) {
        if (!e)
            return;

        removeDrc(e);
        drcElements.insert(drcElements.begin() + pos, e);
    }

    void insert(uint32_t pos, GuiElement *e) {
        insertTv(pos, e);
        insertDrc(pos, e);
    }

    void removeTv(GuiElement *e) {
        for (uint32_t i = 0; i < tvElements.size(); ++i) {
            if (e == tvElements[i]) {
                tvElements.erase(tvElements.begin() + i);
                break;
            }
        }
    }

    void removeDrc(GuiElement *e) {
        for (uint32_t i = 0; i < drcElements.size(); ++i) {
            if (e == drcElements[i]) {
                drcElements.erase(drcElements.begin() + i);
                break;
            }
        }
    }

    void remove(GuiElement *e) {
        removeTv(e);
        removeDrc(e);
    }

    void removeAll() {
        tvElements.clear();
        drcElements.clear();
    }

    void drawDrc(CVideo *video);

    void drawTv(CVideo *video);

    void update(GuiController *controller);

    void updateEffects();

    void process();

    void lockGUI() {
        guiMutex.lock();
    }

    void unlockGUI() {
        guiMutex.unlock();
    }

private:
    void SetupMainView(void);

    void OnOpenEffectFinish(GuiElement *element);

    void OnCloseEffectFinish(GuiElement *element);

    static void OnGameLaunch(uint64_t titleId);

    void OnGameLaunchSplashScreenFinished(GuiElement *element, gameInfo *info, bool launchGame);

    void OnGameLaunchSplashScreen(GuiTitleBrowser *element, uint64_t titleId);

    void OnGameSelectionChange(GuiTitleBrowser *element, uint64_t titleId);

    void OnSettingsButtonClicked(GuiElement *element);

    void OnLayoutSwitchClicked(GuiElement *element);

    void OnLayoutSwitchEffectFinish(GuiElement *element);

    void OnGameListFilterButtonClicked(GuiElement *element);

    void OnGameTitleListChanged(GameList *list);

    void OnGameTitleUpdated(gameInfo *info);

    void OnGameTitleAdded(gameInfo *info);

    int32_t width, height;
    std::vector<GuiElement *> drcElements;
    std::vector<GuiElement *> tvElements;

    GuiSound *gameClickSound;

    MainDrcButtonsFrame *mainSwitchButtonFrame;

    GuiTitleBrowser *currentTvFrame;
    GuiTitleBrowser *currentDrcFrame;

    GuiImageData *pointerImgData[4];
    GuiImage *pointerImg[4];
    bool pointerValid[4];

    GameList gameList;

    std::recursive_mutex guiMutex;
    KeyboardHelper *keyboardInstance = nullptr;
};

#endif //_MAIN_WINDOW_H_
