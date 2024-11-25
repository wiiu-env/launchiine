#ifndef _MAIN_DRC_BUTTONS_FRAME_H
#define _MAIN_DRC_BUTTONS_FRAME_H

#include "gui/Gui.h"
#include "resources/Resources.h"

class MainDrcButtonsFrame : public GuiFrame, public sigslot::has_slots<> {
public:
    MainDrcButtonsFrame(int32_t w, int32_t h)
        : GuiFrame(w, h), buttonClickSound(Resources::GetSound("settings_click_2.mp3")), screenSwitchSound(Resources::GetSound("screenSwitchSound.mp3")),
          switchIconData(Resources::GetImageData("layoutSwitchButton.png")), settingsIconData(Resources::GetImageData("settingsButton.png")), switchIcon(switchIconData),
          settingsIcon(settingsIconData), switchLayoutButton(switchIcon.getWidth(), switchIcon.getHeight()), settingsButton(settingsIcon.getWidth(), settingsIcon.getHeight()),
          gameListFilterButton(w, h), touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH),
          wpadTouchTrigger(GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5, GuiTrigger::BUTTON_A),
          settingsTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_ZL, true), switchLayoutTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_ZR, true),
          plusTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_PLUS, true) {
        settingsButton.setClickable(true);
        settingsButton.setImage(&settingsIcon);
        settingsButton.setTrigger(&touchTrigger);
        settingsButton.setTrigger(&wpadTouchTrigger);
        settingsButton.setTrigger(&settingsTrigger);
        settingsButton.setAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
        settingsButton.setSoundClick(buttonClickSound);
        settingsButton.setEffectGrow();
        settingsButton.clicked.connect(this, &MainDrcButtonsFrame::OnSettingsButtonClick);
        append(&settingsButton);

        switchLayoutButton.setClickable(true);
        switchLayoutButton.setImage(&switchIcon);
        switchLayoutButton.setTrigger(&touchTrigger);
        switchLayoutButton.setTrigger(&wpadTouchTrigger);
        switchLayoutButton.setTrigger(&switchLayoutTrigger);
        switchLayoutButton.setAlignment(ALIGN_RIGHT | ALIGN_BOTTOM);
        switchLayoutButton.setSoundClick(screenSwitchSound);
        switchLayoutButton.setEffectGrow();
        switchLayoutButton.clicked.connect(this, &MainDrcButtonsFrame::OnLayoutSwithClick);
        append(&switchLayoutButton);

        gameListFilterButton.setClickable(true);
        gameListFilterButton.setSoundClick(buttonClickSound);
        gameListFilterButton.setTrigger(&plusTrigger);
        gameListFilterButton.clicked.connect(this, &MainDrcButtonsFrame::OnGameListFilterButtonClicked);
        append(&gameListFilterButton);
    }

    virtual ~MainDrcButtonsFrame() {
        Resources::RemoveImageData(switchIconData);
        Resources::RemoveImageData(settingsIconData);
        Resources::RemoveSound(buttonClickSound);
        Resources::RemoveSound(screenSwitchSound);
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

    GuiSound *buttonClickSound;
    GuiSound *screenSwitchSound;
    GuiImageData *switchIconData;
    GuiImageData *settingsIconData;
    GuiImage switchIcon;
    GuiImage settingsIcon;

    GuiButton switchLayoutButton;
    GuiButton settingsButton;
    GuiButton gameListFilterButton;

    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;
    GuiTrigger settingsTrigger;
    GuiTrigger switchLayoutTrigger;
    GuiTrigger plusTrigger;
};

#endif //_SETTINGS_WINDOW_H_