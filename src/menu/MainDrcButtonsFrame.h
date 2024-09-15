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
#ifndef _MAIN_DRC_BUTTONS_FRAME_H_
#define _MAIN_DRC_BUTTONS_FRAME_H_

#include "gui/Gui.h"
#include "resources/Resources.h"

class MainDrcButtonsFrame : public GuiFrame, public sigslot::has_slots<> {
public:
    MainDrcButtonsFrame(int32_t w, int32_t h);

    ~MainDrcButtonsFrame() override {
    }

    sigslot::signal1<GuiElement *> settingsButtonClicked;
    sigslot::signal1<GuiElement *> layoutSwitchClicked;
    sigslot::signal1<GuiElement *> gameListFilterClicked;

private:
    void OnSettingsButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *) {
        settingsButtonClicked(this);
    }

    void OnLayoutSwithClick(GuiButton *button, const GuiController *controller, GuiTrigger *) {
        layoutSwitchClicked(this);
    }

    void OnGameListFilterButtonClicked(GuiButton *button, const GuiController *controller, GuiTrigger *) {
        gameListFilterClicked(this);
    }

    std::unique_ptr<GuiSound> buttonClickSound;
    std::unique_ptr<GuiSound> screenSwitchSound;
    std::shared_ptr<GuiImage> switchIcon;
    std::shared_ptr<GuiImage> settingsIcon;

    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;
    GuiTrigger settingsTrigger;
    GuiTrigger switchLayoutTrigger;
    GuiTrigger plusTrigger;
};

#endif //_SETTINGS_WINDOW_H_
