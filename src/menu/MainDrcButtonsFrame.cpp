
#include "MainDrcButtonsFrame.h"

MainDrcButtonsFrame::MainDrcButtonsFrame(int32_t w, int32_t h)
    : GuiFrame(w, h),
      buttonClickSound(Resources::GetSound("settings_click_2.mp3")),
      screenSwitchSound(Resources::GetSound("screenSwitchSound.mp3")),
      switchIcon(std::make_unique<GuiImage>(Resources::GetImageData("layoutSwitchButton.png"))),
      settingsIcon(std::make_unique<GuiImage>(Resources::GetImageData("settingsButton->png"))),
      touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH),
      wpadTouchTrigger(GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5, GuiTrigger::BUTTON_A),
      settingsTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_ZL, true), switchLayoutTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_ZR, true),
      plusTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_PLUS, true) {


    auto settingsButton = std::make_unique<GuiButton>(settingsIcon->getWidth(), settingsIcon->getHeight());
    settingsButton->setClickable(true);
    settingsButton->setImage(settingsIcon);
    settingsButton->setTrigger(&touchTrigger);
    settingsButton->setTrigger(&wpadTouchTrigger);
    settingsButton->setTrigger(&settingsTrigger);
    settingsButton->setAlignment(ALIGN_LEFT | ALIGN_BOTTOM);
    settingsButton->setSoundClick(std::move(buttonClickSound));
    settingsButton->setEffectGrow();
    settingsButton->clicked.connect(this, &MainDrcButtonsFrame::OnSettingsButtonClick);
    append(std::move(settingsButton));

    auto switchLayoutButton = std::make_unique<GuiButton>(switchIcon->getWidth(), switchIcon->getHeight());
    switchLayoutButton->setClickable(true);
    switchLayoutButton->setImage(switchIcon);
    switchLayoutButton->setTrigger(&touchTrigger);
    switchLayoutButton->setTrigger(&wpadTouchTrigger);
    switchLayoutButton->setTrigger(&switchLayoutTrigger);
    switchLayoutButton->setAlignment(ALIGN_RIGHT | ALIGN_BOTTOM);
    switchLayoutButton->setSoundClick(std::move(screenSwitchSound));
    switchLayoutButton->setEffectGrow();
    switchLayoutButton->clicked.connect(this, &MainDrcButtonsFrame::OnLayoutSwithClick);
    append(std::move(switchLayoutButton));

    auto gameListFilterButton = std::make_unique<GuiButton>(w, h);
    gameListFilterButton->setClickable(true);
    gameListFilterButton->setSoundClick(std::move(buttonClickSound));
    gameListFilterButton->setTrigger(&plusTrigger);
    gameListFilterButton->clicked.connect(this, &MainDrcButtonsFrame::OnGameListFilterButtonClicked);
    append(std::move(gameListFilterButton));
}
