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

    void appendTv(std::shared_ptr<GuiElement> e) {
        if (!e)
            return;

        removeTvByPtr(e.get());
        tvElements.push_back(std::move(e));
    }

    void appendDrc(std::shared_ptr<GuiElement> e) {
        if (!e)
            return;

        removeDrcByPtr(e.get());
        drcElements.push_back(std::move(e));
    }

    void append(const std::shared_ptr<GuiElement> &e) {
        appendTv(e);
        appendDrc(e);
    }

    void insertTv(uint32_t pos, const std::shared_ptr<GuiElement> &e) {
        if (!e)
            return;

        removeTvByPtr(e.get());
        tvElements.insert(tvElements.begin() + pos, e);
    }

    void insertDrc(uint32_t pos, const std::shared_ptr<GuiElement> &e) {
        if (!e)
            return;

        removeDrcByPtr(e.get());
        drcElements.insert(drcElements.begin() + pos, e);
    }

    void insert(uint32_t pos, const std::shared_ptr<GuiElement> &e) {
        insertTv(pos, e);
        insertDrc(pos, e);
    }

    void removeTvByPtr(GuiElement *e) {
        for (uint32_t i = 0; i < tvElements.size(); ++i) {
            if (e == tvElements[i].get()) {
                tvElements.erase(tvElements.begin() + i);
                break;
            }
        }
    }

    void removeDrcByPtr(GuiElement *e) {
        for (uint32_t i = 0; i < drcElements.size(); ++i) {
            if (e == drcElements[i].get()) {
                drcElements.erase(drcElements.begin() + i);
                break;
            }
        }
    }

    void removeByPtr(GuiElement *e) {
        removeTvByPtr(e);
        removeDrcByPtr(e);
    }

    void removeAll() {
        tvElements.clear();
        drcElements.clear();
    }

    void drawDrc(const CVideo &video);

    void drawTv(const CVideo &video);

    void update(const GuiController &controller);

    void updateEffects();

    void process();

    void lockGUI() {
        guiMutex.lock();
    }

    void unlockGUI() {
        guiMutex.unlock();
    }

private:
    void SetupMainView();

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
    std::vector<std::shared_ptr<GuiElement>> drcElements;
    std::vector<std::shared_ptr<GuiElement>> tvElements;

    std::shared_ptr<MainDrcButtonsFrame> mainSwitchButtonFrame;

    std::shared_ptr<GuiTitleBrowser> currentTvFrame;
    std::shared_ptr<GuiTitleBrowser> currentDrcFrame;

    std::array<std::unique_ptr<GuiImage>, 4> pointerImg;
    bool pointerValid[4] = {};

    GameList gameList;

    std::recursive_mutex guiMutex;
    KeyboardHelper *keyboardInstance = nullptr;
};

#endif //_MAIN_WINDOW_H_
