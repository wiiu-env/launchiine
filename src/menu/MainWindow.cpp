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
#include "MainWindow.h"
#include "Application.h"
#include "utils/logger.h"
#include "utils/StringTools.h"

#include "resources/Resources.h"
#include "gui/GuiTitleBrowser.h"
#include "gui/GuiIconGrid.h"
#include <sysapp/launch.h>
#include <future>
#include "utils/AsyncExecutor.h"

MainWindow::MainWindow(int32_t w, int32_t h)
    : width(w)
    , height(h)
    , gameClickSound(Resources::GetSound("game_click.mp3"))
    , mainSwitchButtonFrame(NULL)
    , currentTvFrame(NULL)
    , currentDrcFrame(NULL) {
    for(int32_t i = 0; i < 4; i++) {
        std::string filename = StringTools::strfmt("player%i_point.png", i+1);
        pointerImgData[i] = Resources::GetImageData(filename.c_str());
        pointerImg[i] = new GuiImage(pointerImgData[i]);
        pointerImg[i]->setScale(1.5f);
        pointerValid[i] = false;
    }
    SetupMainView();
    gameList.titleListChanged.connect(this, &MainWindow::OnGameTitleListChanged);
    gameList.titleUpdated.connect(this, &MainWindow::OnGameTitleUpdated);
    gameList.titleAdded.connect(this, &MainWindow::OnGameTitleAdded);
    AsyncExecutor::execute([&] {gameList.loadUnfiltered();});

}

MainWindow::~MainWindow() {
    gameList.titleListChanged.disconnect(this);
    gameList.titleUpdated.disconnect(this);
    gameList.titleAdded.disconnect(this);
    while(!tvElements.empty()) {
        delete tvElements[0];
        remove(tvElements[0]);
    }
    while(!drcElements.empty()) {
        delete drcElements[0];
        remove(drcElements[0]);
    }
    for(int32_t i = 0; i < 4; i++) {
        delete pointerImg[i];
        Resources::RemoveImageData(pointerImgData[i]);
    }

    Resources::RemoveSound(gameClickSound);
}

void MainWindow::updateEffects() {
    //! dont read behind the initial elements in case one was added
    uint32_t tvSize = tvElements.size();
    uint32_t drcSize = drcElements.size();

    for(uint32_t i = 0; (i < drcSize) && (i < drcElements.size()); ++i) {
        drcElements[i]->updateEffects();
    }

    //! only update TV elements that are not updated yet because they are on DRC
    for(uint32_t i = 0; (i < tvSize) && (i < tvElements.size()); ++i) {
        uint32_t n;
        for(n = 0; (n < drcSize) && (n < drcElements.size()); n++) {
            if(tvElements[i] == drcElements[n])
                break;
        }
        if(n == drcElements.size()) {
            tvElements[i]->updateEffects();
        }
    }
}

void MainWindow::process() {
    //! dont read behind the initial elements in case one was added
    uint32_t tvSize = tvElements.size();
    uint32_t drcSize = drcElements.size();

    for(uint32_t i = 0; (i < drcSize) && (i < drcElements.size()); ++i) {
        drcElements[i]->process();
    }

    //! only update TV elements that are not updated yet because they are on DRC
    for(uint32_t i = 0; (i < tvSize) && (i < tvElements.size()); ++i) {
        uint32_t n;
        for(n = 0; (n < drcSize) && (n < drcElements.size()); n++) {
            if(tvElements[i] == drcElements[n])
                break;
        }
        if(n == drcElements.size()) {
            tvElements[i]->process();
        }
    }

    if(keyboardInstance != NULL) {
        if(keyboardInstance->checkResult()) {
            std::string result = keyboardInstance->getResult();

            currentTvFrame->clearState(GuiElement::STATE_DISABLED);
            currentDrcFrame->clearState(GuiElement::STATE_DISABLED);
            mainSwitchButtonFrame->clearState(GuiElement::STATE_DISABLED);

            gameList.filterList(result.c_str());
        }else{
        }
    }
}

void MainWindow::OnGameTitleListChanged(GameList * list) {
    currentTvFrame->OnGameTitleListUpdated(list);
    if(currentTvFrame != currentDrcFrame) {
        currentDrcFrame->OnGameTitleListUpdated(list);
    }
}

void MainWindow::OnGameTitleUpdated(gameInfo * info) {
    currentTvFrame->OnGameTitleUpdated(info);
    if(currentTvFrame != currentDrcFrame) {
        currentDrcFrame->OnGameTitleUpdated(info);
    }
}

void MainWindow::OnGameTitleAdded(gameInfo * info) {
    currentTvFrame->OnGameTitleAdded(info);
    if(currentTvFrame != currentDrcFrame) {
        currentDrcFrame->OnGameTitleAdded(info);
    }
}

void MainWindow::update(GuiController *controller) {
    //! dont read behind the initial elements in case one was added
    //uint32_t tvSize = tvElements.size();

    if(controller->chan & GuiTrigger::CHANNEL_1) {
        uint32_t drcSize = drcElements.size();

        for(uint32_t i = 0; (i < drcSize) && (i < drcElements.size()); ++i) {
            drcElements[i]->update(controller);
        }
    } else {
        uint32_t tvSize = tvElements.size();

        for(uint32_t i = 0; (i < tvSize) && (i < tvElements.size()); ++i) {
            tvElements[i]->update(controller);
        }
    }

//    //! only update TV elements that are not updated yet because they are on DRC
//    for(uint32_t i = 0; (i < tvSize) && (i < tvElements.size()); ++i)
//    {
//        uint32_t n;
//        for(n = 0; (n < drcSize) && (n < drcElements.size()); n++)
//        {
//            if(tvElements[i] == drcElements[n])
//                break;
//        }
//        if(n == drcElements.size())
//        {
//            tvElements[i]->update(controller);
//        }
//    }

    if(controller->chanIdx >= 1 && controller->chanIdx <= 4 && controller->data.validPointer) {
        int32_t wpadIdx = controller->chanIdx - 1;
        float posX = controller->data.x;
        float posY = controller->data.y;
        pointerImg[wpadIdx]->setPosition(posX, posY);
        pointerImg[wpadIdx]->setAngle(controller->data.pointerAngle);
        pointerValid[wpadIdx] = true;
    }
}

void MainWindow::drawDrc(CVideo *video) {
    for(uint32_t i = 0; i < drcElements.size(); ++i) {
        drcElements[i]->draw(video);
    }

    for(int32_t i = 0; i < 4; i++) {
        if(pointerValid[i]) {
            pointerImg[i]->setAlpha(0.5f);
            pointerImg[i]->draw(video);
            pointerImg[i]->setAlpha(1.0f);
        }
    }

    if(keyboardInstance != NULL) {
        keyboardInstance->drawDRC();
    }
}

void MainWindow::drawTv(CVideo *video) {
    for(uint32_t i = 0; i < tvElements.size(); ++i) {
        tvElements[i]->draw(video);
    }

    for(int32_t i = 0; i < 4; i++) {
        if(pointerValid[i]) {
            pointerImg[i]->draw(video);
            pointerValid[i] = false;
        }
    }
    if(keyboardInstance != NULL) {
        keyboardInstance->drawTV();
    }
}

void MainWindow::SetupMainView() {
    currentTvFrame = new GuiIconGrid(width, height,0, true);

    currentTvFrame->setEffect(EFFECT_FADE, 10, 255);
    currentTvFrame->setState(GuiElement::STATE_DISABLED);
    currentTvFrame->effectFinished.connect(this, &MainWindow::OnOpenEffectFinish);

    appendTv(currentTvFrame);

    currentDrcFrame = new GuiIconGrid(width, height,0, false);
    currentDrcFrame->setEffect(EFFECT_FADE, 10, 255);
    currentDrcFrame->setState(GuiElement::STATE_DISABLED);
    currentDrcFrame->effectFinished.connect(this, &MainWindow::OnOpenEffectFinish);

    if(currentTvFrame != currentDrcFrame) {
        currentDrcFrame->setEffect(EFFECT_FADE, 10, 255);
        currentDrcFrame->setState(GuiElement::STATE_DISABLED);
        currentDrcFrame->effectFinished.connect(this, &MainWindow::OnOpenEffectFinish);
    }

    //! reconnect only to DRC game selection change
    currentTvFrame->gameSelectionChanged.disconnect(this);
    currentDrcFrame->gameSelectionChanged.disconnect(this);
    currentTvFrame->gameLaunchClicked.disconnect(this);
    currentDrcFrame->gameLaunchClicked.disconnect(this);


    if(currentTvFrame != currentDrcFrame) {
        currentTvFrame->gameSelectionChanged.connect(this, &MainWindow::OnGameSelectionChange);
        currentTvFrame->gameLaunchClicked.connect(this, &MainWindow::OnGameLaunch);
    }

    currentDrcFrame->gameSelectionChanged.connect(this, &MainWindow::OnGameSelectionChange);
    currentDrcFrame->gameLaunchClicked.connect(this, &MainWindow::OnGameLaunch);

    mainSwitchButtonFrame = new MainDrcButtonsFrame(width, height);
    mainSwitchButtonFrame->settingsButtonClicked.connect(this, &MainWindow::OnSettingsButtonClicked);
    mainSwitchButtonFrame->layoutSwitchClicked.connect(this, &MainWindow::OnLayoutSwitchClicked);
    mainSwitchButtonFrame->gameListFilterClicked.connect(this, &MainWindow::OnGameListFilterButtonClicked);
    mainSwitchButtonFrame->setState(GuiElement::STATE_DISABLED);
    mainSwitchButtonFrame->setEffect(EFFECT_FADE, 10, 255);
    mainSwitchButtonFrame->setState(GuiElement::STATE_DISABLED);
    mainSwitchButtonFrame->effectFinished.connect(this, &MainWindow::OnOpenEffectFinish);

    appendDrc(currentDrcFrame);
    append(mainSwitchButtonFrame);
}

void MainWindow::OnLayoutSwitchClicked(GuiElement *element) {
    if(!currentTvFrame || !currentDrcFrame || !mainSwitchButtonFrame) {
        return;
    }

    if(currentTvFrame == currentDrcFrame) {
        return;
    }

    currentTvFrame->setState(GuiElement::STATE_DISABLED);
    currentTvFrame->setEffect(EFFECT_FADE, -15, 0);
    currentTvFrame->effectFinished.connect(this, &MainWindow::OnLayoutSwitchEffectFinish);

    currentDrcFrame->setState(GuiElement::STATE_DISABLED);
    currentDrcFrame->setEffect(EFFECT_FADE, -15, 0);

    mainSwitchButtonFrame->setState(GuiElement::STATE_DISABLED);
}

void MainWindow::OnGameListFilterButtonClicked(GuiElement *element) {
    if(!currentTvFrame || !currentDrcFrame || !mainSwitchButtonFrame) {
        return;
    }

    if(keyboardInstance == NULL) {
        keyboardInstance = new KeyboardHelper();
    }
    if(keyboardInstance->isReady()) {
        if(keyboardInstance->openKeyboard()) {
            currentTvFrame->setState(GuiElement::STATE_DISABLED);
            currentDrcFrame->setState(GuiElement::STATE_DISABLED);
            mainSwitchButtonFrame->setState(GuiElement::STATE_DISABLED);
        }
    }
}

void MainWindow::OnLayoutSwitchEffectFinish(GuiElement *element) {
    if(!currentTvFrame || !currentDrcFrame || !mainSwitchButtonFrame)
        return;

    element->effectFinished.disconnect(this);
    remove(currentDrcFrame);
    remove(currentTvFrame);

    GuiTitleBrowser *tmpElement = currentDrcFrame;
    currentDrcFrame = currentTvFrame;
    currentTvFrame = tmpElement;

    appendTv(currentTvFrame);
    appendDrc(currentDrcFrame);
    //! re-append on top
    append(mainSwitchButtonFrame);

    currentTvFrame->resetState();
    currentTvFrame->setEffect(EFFECT_FADE, 15, 255);

    currentDrcFrame->resetState();
    currentDrcFrame->setEffect(EFFECT_FADE, 15, 255);

    mainSwitchButtonFrame->clearState(GuiElement::STATE_DISABLED);

    //! reconnect only to DRC game selection change
    currentTvFrame->gameSelectionChanged.disconnect(this);
    currentDrcFrame->gameSelectionChanged.disconnect(this);
    currentTvFrame->gameLaunchClicked.disconnect(this);
    currentDrcFrame->gameLaunchClicked.disconnect(this);

    currentTvFrame->gameSelectionChanged.connect(this, &MainWindow::OnGameSelectionChange);
    currentTvFrame->gameLaunchClicked.connect(this, &MainWindow::OnGameLaunch);
    currentDrcFrame->gameSelectionChanged.connect(this, &MainWindow::OnGameSelectionChange);
    currentDrcFrame->gameLaunchClicked.connect(this, &MainWindow::OnGameLaunch);
}

void MainWindow::OnOpenEffectFinish(GuiElement *element) {
    //! once the menu is open reset its state and allow it to be "clicked/hold"
    element->effectFinished.disconnect(this);
    element->clearState(GuiElement::STATE_DISABLED);
}

void MainWindow::OnCloseEffectFinish(GuiElement *element) {
    //! remove element from draw list and push to delete queue
    remove(element);
    AsyncExecutor::pushForDelete(element);
}

void MainWindow::OnSettingsButtonClicked(GuiElement *element) {

}

void MainWindow::OnGameSelectionChange(GuiTitleBrowser *element, uint64_t selectedIdx) {
    if(!currentDrcFrame || !currentTvFrame)
        return;

    if(element == currentDrcFrame && currentDrcFrame != currentTvFrame) {
        currentTvFrame->setSelectedGame(selectedIdx);
    } else if(element == currentTvFrame && currentDrcFrame != currentTvFrame) {
        currentDrcFrame->setSelectedGame(selectedIdx);
    }
}

void MainWindow::OnGameLaunch(GuiTitleBrowser *element, uint64_t selectedIdx) {

}
