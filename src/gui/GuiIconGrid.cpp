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
#include <map>
#include <algorithm>
#include <gui/GuiIconGrid.h>
#include <gui/GuiController.h>
#include <coreinit/cache.h>
#include "common/common.h"
#include "Application.h"
#include <gui/video/CVideo.h>
#include "utils/logger.h"
#include "gui/GameIcon.h"

GuiIconGrid::GuiIconGrid(int32_t w, int32_t h, uint64_t GameIndex,bool sortByName)
    : GuiTitleBrowser(w, h, GameIndex),
      sortByName(sortByName),
      particleBgImage(w, h, 50, 60.0f, 90.0f, 0.6f, 1.0f)
    , buttonClickSound(Resources::GetSound("button_click.mp3"))
    , touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH)
    , wpadTouchTrigger(GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5, GuiTrigger::BUTTON_A)
    , leftTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_LEFT | GuiTrigger::STICK_L_LEFT, true)
    , rightTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_RIGHT | GuiTrigger::STICK_L_RIGHT, true)
    , downTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_DOWN | GuiTrigger::STICK_L_DOWN, true)
    , upTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_UP | GuiTrigger::STICK_L_UP, true)
    , buttonATrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_A, true)
    , buttonLTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_L, true)
    , buttonRTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_R, true)
    , leftButton(w, h)
    , rightButton(w, h)
    , downButton(w, h)
    , upButton(w, h)
    , launchButton(w, h)
    , arrowRightImageData(Resources::GetImageData("rightArrow.png"))
    , arrowLeftImageData(Resources::GetImageData("leftArrow.png"))
    , arrowRightImage(arrowRightImageData)
    , arrowLeftImage(arrowLeftImageData)
    , arrowRightButton(arrowRightImage.getWidth(), arrowRightImage.getHeight())
    , arrowLeftButton(arrowLeftImage.getWidth(), arrowLeftImage.getHeight())
    , noIcon(Resources::GetFile("noGameIcon.png"), Resources::GetFileSize("noGameIcon.png"), GX2_TEX_CLAMP_MODE_MIRROR)
    , emptyIcon(Resources::GetFile("iconEmpty.png"), Resources::GetFileSize("iconEmpty.png"), GX2_TEX_CLAMP_MODE_MIRROR)
    , dragListener(w,h) {

    particleBgImage.setParent(this);
    setSelectedGame(GameIndex);
    listOffset = selectedGame / (MAX_COLS * MAX_ROWS);
    targetLeftPosition = -listOffset * getWidth();
    currentLeftPosition = targetLeftPosition;

    leftButton.setTrigger(&leftTrigger);
    leftButton.clicked.connect(this, &GuiIconGrid::OnLeftClick);
    this->append(&leftButton);

    rightButton.setTrigger(&rightTrigger);
    rightButton.clicked.connect(this, &GuiIconGrid::OnRightClick);
    this->append(&rightButton);

    downButton.setTrigger(&downTrigger);
    downButton.clicked.connect(this, &GuiIconGrid::OnDownClick);
    this->append(&downButton);

    upButton.setTrigger(&upTrigger);
    upButton.clicked.connect(this, &GuiIconGrid::OnUpClick);
    this->append(&upButton);

    launchButton.setTrigger(&buttonATrigger);
    launchButton.setSoundClick(buttonClickSound);
    launchButton.clicked.connect(this, &GuiIconGrid::OnLaunchClick);
    this->append(&launchButton);

    arrowLeftButton.setImage(&arrowLeftImage);
    arrowLeftButton.setEffectGrow();
    arrowLeftButton.setPosition(40, 0);
    arrowLeftButton.setAlignment(ALIGN_LEFT | ALIGN_MIDDLE);
    arrowLeftButton.setTrigger(&touchTrigger);
    arrowLeftButton.setTrigger(&wpadTouchTrigger);
    arrowLeftButton.setTrigger(&buttonLTrigger);
    arrowLeftButton.setHoldable(true);
    arrowLeftButton.setSoundClick(buttonClickSound);
    arrowLeftButton.clicked.connect(this, &GuiIconGrid::OnLeftArrowClick);
    arrowLeftButton.held.connect(this, &GuiIconGrid::OnLeftArrowHeld);
    arrowLeftButton.released.connect(this, &GuiIconGrid::OnLeftArrowReleased);

    append(&arrowLeftButton);

    arrowRightButton.setImage(&arrowRightImage);
    arrowRightButton.setEffectGrow();
    arrowRightButton.setPosition(-40, 0);
    arrowRightButton.setAlignment(ALIGN_RIGHT | ALIGN_MIDDLE);
    arrowRightButton.setTrigger(&touchTrigger);
    arrowRightButton.setTrigger(&wpadTouchTrigger);
    arrowRightButton.setTrigger(&buttonRTrigger);
    arrowRightButton.setHoldable(true);
    arrowRightButton.setSoundClick(buttonClickSound);
    arrowRightButton.clicked.connect(this, &GuiIconGrid::OnRightArrowClick);
    arrowRightButton.held.connect(this, &GuiIconGrid::OnRightArrowHeld);
    arrowRightButton.released.connect(this, &GuiIconGrid::OnRightArrowReleased);
    append(&arrowRightButton);

    // at most we are rendering 2 screens at the same time
    for(int i = 0; i< MAX_COLS * MAX_ROWS *2; i++) {
        GameIcon * image = new GameIcon(&emptyIcon);
        emptyIcons.push_back(image);
    }

    dragListener.setTrigger(&touchTrigger);
    dragListener.setTrigger(&wpadTouchTrigger);
    dragListener.dragged.connect(this, &GuiIconGrid::OnDrag);

    append(&dragListener);
}

GuiIconGrid::~GuiIconGrid() {
    containerMutex.lock();
    for (auto const& x : gameInfoContainers) {
        remove(x.second->button);
        delete x.second;
    }
    gameInfoContainers.clear();
    containerMutex.unlock();

    for (auto const& x : emptyIcons) {
        AsyncExecutor::pushForDelete(x);
    }

    emptyIcons.clear();
}

int32_t GuiIconGrid::offsetForTitleId(uint64_t titleId) {
    int32_t offset = -1;
    for(uint32_t i = 0; i < position.size(); i++) {
        if(position.at(i) == titleId) {
            offset = i;
            break;
        }
    }
    return offset;
}

void GuiIconGrid::setSelectedGame(uint64_t idx) {
    this->selectedGame = idx;

    containerMutex.lock();
    GameInfoContainer * container = NULL;
    for (auto const& x : gameInfoContainers) {
        container = x.second;
        if(x.first == idx) {
            container->image->setSelected(true);
        } else {
            container->image->setSelected(false);
        }
    }
    containerMutex.unlock();

    int32_t offset = offsetForTitleId(getSelectedGame());
    if(offset > 0) {
        uint32_t newPage = offset / (MAX_COLS * MAX_ROWS);
        if(newPage != (uint32_t) curPage) {
            curPage = newPage;
            bUpdatePositions = true;
        }
    }
}

uint64_t GuiIconGrid::getSelectedGame(void) {
    return selectedGame;
}

void GuiIconGrid::OnGameTitleListUpdated(GameList * gameList) {
    gameList->lock();
    containerMutex.lock();
    // At first delete the ones that were deleted;
    auto it = gameInfoContainers.begin();
    while (it != gameInfoContainers.end()) {
        bool wasFound = false;
        for(int32_t i = 0; i < gameList->size(); i++) {
            gameInfo * info = gameList->at(i);
            if(info != NULL && info->titleId == it->first) {
                wasFound = true;
                break;
            }
        }

        if (!wasFound) {
            DEBUG_FUNCTION_LINE("Removing %016llX\n", it->first);
            remove(it->second->button);
            delete it->second;
            it = gameInfoContainers.erase(it);
        } else {
            ++it;
        }
    }

    for(int32_t i = 0; i < gameList->size(); i++) {
        gameInfo * info = gameList->at(i);
        GameInfoContainer * container = NULL;

        for (auto const& x : gameInfoContainers) {
            if(info->titleId ==  x.first) {
                container = x.second;
                break;
            }
        }
        if(container == NULL) {
            OnGameTitleAdded(info);
        }
    }
    containerMutex.unlock();

    gameList->unlock();
    setSelectedGame(0);
    gameSelectionChanged(this, selectedGame);
    curPage = 0;
    currentLeftPosition = 0;
    bUpdatePositions = true;
}

void GuiIconGrid::OnLeftArrowClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    //setSelectedGame(0);
    curPage--;
    bUpdatePositions = true;
}

void GuiIconGrid::OnRightArrowClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    //setSelectedGame(0);
    curPage++;
    bUpdatePositions = true;
}

void GuiIconGrid::OnLeftClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    int32_t offset = offsetForTitleId(getSelectedGame());
    if(offset < 0) {
        return;
    }
    if((offset % MAX_COLS) == 0) {
        offset -= ((MAX_COLS * MAX_ROWS) - MAX_COLS) + 1;
    } else {
        offset--;
    }
    if(offset < 0 || position.empty()) {
        return;
    }
    uint64_t newTitleId = position.at(offset);
    if(newTitleId > 0) {
        setSelectedGame(newTitleId);
        gameSelectionChanged(this, selectedGame);
    }

}

void GuiIconGrid::OnRightClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    int32_t offset = offsetForTitleId(getSelectedGame());
    if(offset < 0) {
        return;
    }
    if((offset % MAX_COLS) == MAX_COLS - 1) {
        offset += ((MAX_COLS * MAX_ROWS) - MAX_COLS) + 1;
    } else {
        offset++;
    }
    if((uint32_t) offset >= position.size()) {
        return;
    }
    uint64_t newTitleId = position.at(offset);
    if(newTitleId > 0) {
        setSelectedGame(newTitleId);
        gameSelectionChanged(this, selectedGame);
    }
}

void GuiIconGrid::OnDownClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    int32_t offset = offsetForTitleId(getSelectedGame());
    if(offset < 0) {
        return;
    }
    if(offset % (MAX_COLS * MAX_ROWS) < (MAX_COLS * MAX_ROWS) - MAX_COLS) {
        offset = offset + MAX_COLS;
    } else {
        return;
    }

    if((uint32_t) offset >= position.size()) {
        return;
    }
    uint64_t newTitleId = position.at(offset);
    if(newTitleId > 0) {
        setSelectedGame(newTitleId);
        gameSelectionChanged(this, selectedGame);
    }
}
void GuiIconGrid::OnUpClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    int32_t offset = offsetForTitleId(getSelectedGame());
    if(offset < 0) {
        return;
    }
    if(offset % (MAX_COLS * MAX_ROWS) >= MAX_COLS) {
        offset = offset - MAX_COLS;
    } else {
        return;
    }

    if(offset < 0) {
        return;
    }
    uint64_t newTitleId = position.at(offset);
    if(newTitleId > 0) {
        setSelectedGame(newTitleId);
        gameSelectionChanged(this, selectedGame);
    }
}

void GuiIconGrid::OnLaunchClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    //! do not auto launch when wiimote is pointing to screen and presses A
    if((trigger == &buttonATrigger) && (controller->chan & (GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5)) && controller->data.validPointer) {
        return;
    }
    DEBUG_FUNCTION_LINE("Tried to launch %s (%016llX)\n", gameInfoContainers[getSelectedGame()]->info->name.c_str(),getSelectedGame());
    gameLaunchClicked(this, getSelectedGame());
}


void GuiIconGrid::OnLeftArrowHeld(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    if(currentlyHeld != NULL) {
        if(lArrowHeldCounter++ > 30) {
            OnLeftArrowClick(button,controller,trigger);
            lArrowHeldCounter = 0;
        }
    } else {
        lArrowHeldCounter = 0;
    }
}

void GuiIconGrid::OnLeftArrowReleased(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    lArrowHeldCounter = 0;
}

void GuiIconGrid::OnRightArrowHeld(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    if(currentlyHeld != NULL) {
        if(rArrowHeldCounter++ > 30) {
            DEBUG_FUNCTION_LINE("CLICK\n");
            OnRightArrowClick(button,controller,trigger);
            rArrowHeldCounter = 0;
        }
    } else {
        rArrowHeldCounter = 0;
    }
}

void GuiIconGrid::OnRightArrowReleased(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    rArrowHeldCounter = 0;
}


void GuiIconGrid::OnGameButtonHeld(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    if(currentlyHeld == NULL) {
        currentlyHeld = button;
    }
    if(currentlyHeld != NULL && currentlyHeld != button) {
        dragTarget = button;
    }
}

void GuiIconGrid::OnGameButtonPointedOn(GuiButton *button, const GuiController *controller) {

}

void GuiIconGrid::OnGameButtonPointedOff(GuiButton *button, const GuiController *controller) {

}

void GuiIconGrid::OnDrag(GuiDragListener  * element, const GuiController *controller, GuiTrigger *trigger, int32_t dx, int32_t dy) {
    if(currentlyHeld != NULL) {
        currentlyHeld->setPosition(currentlyHeld->getOffsetX() + dx, currentlyHeld->getOffsetY() + dy);
    }
    // reset the target when we move.
    dragTarget = NULL;
}

void GuiIconGrid::OnGameButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    containerMutex.lock();
    for (auto const& x : gameInfoContainers) {
        if(x.second->button == button) {
            if(selectedGame == (x.second->info->titleId)) {
                if(gameLaunchTimer < 30)
                    OnLaunchClick(button, controller, trigger);
            } else {
                setSelectedGame(x.second->info->titleId);
                gameSelectionChanged(this, selectedGame);
            }
            gameLaunchTimer = 0;
            break;
        }
    }
    containerMutex.unlock();
}

void GuiIconGrid::OnGameTitleAdded(gameInfo * info) {
    DEBUG_FUNCTION_LINE("Adding %016llX\n", info->titleId);
    GuiImageData * imageData = &noIcon;
    if(info->imageData != NULL) {
        imageData = info->imageData;
    }
    GameIcon * image = new GameIcon(imageData);
    image->setRenderReflection(false);
    image->setStrokeRender(false);
    image->setSelected(info->titleId == selectedGame);
    image->setRenderIconLast(true);

    GuiButton * button = new GuiButton(noIcon.getWidth(), noIcon.getHeight());
    button->setImage(image);
    button->setPosition(0, 0);
    button->setEffectGrow();
    button->setTrigger(&touchTrigger);
    button->setTrigger(&wpadTouchTrigger);
    button->setSoundClick(buttonClickSound);
    //button->setClickable( (idx < gameList->size()) );
    //button->setSelectable( (idx < gameList->size()) );
    button->clicked.connect(this, &GuiIconGrid::OnGameButtonClick);
    button->setHoldable(true);
    button->held.connect(this, &GuiIconGrid::OnGameButtonHeld);
    button->pointedOn.connect(this, &GuiIconGrid::OnGameButtonPointedOn);
    button->pointedOff.connect(this, &GuiIconGrid::OnGameButtonPointedOff);
    //button->dragged.connect(this, &GuiIconGrid::OnGameButtonDragged);

    GameInfoContainer * container = new GameInfoContainer(button, image, info);
    containerMutex.lock();
    gameInfoContainers[info->titleId] = container;
    containerMutex.unlock();
    this->append(button);

    position.push_back(info->titleId);

    bUpdatePositions = true;
}
void GuiIconGrid::OnGameTitleUpdated(gameInfo * info) {
    DEBUG_FUNCTION_LINE("Updating infos of %016llX\n", info->titleId);
    GameInfoContainer * container = NULL;
    containerMutex.lock();
    for (auto const& x : gameInfoContainers) {
        if(info->titleId == x.first) {
            container = x.second;
            break;
        }
    }

    // keep the lock to delay the draw() until the image data is ready.
    if(container != NULL) {
        container->updateImageData();
    }

    containerMutex.unlock();

    bUpdatePositions = true;
}

void GuiIconGrid::process() {
    if(currentlyHeld != NULL) {
        if(!currentlyHeld->isStateSet(GuiElement::STATE_HELD)) {
            DEBUG_FUNCTION_LINE("Not held anymore\n");

            if(dragTarget) {
                DEBUG_FUNCTION_LINE("Let's swap\n");

                std::vector<std::pair<uint64_t,GameInfoContainer*>> vec;
                containerMutex.lock();
                // copy key-value pairs from the map to the vector
                std::copy(gameInfoContainers.begin(), gameInfoContainers.end(), std::back_inserter<std::vector<std::pair<uint64_t,GameInfoContainer*>>>(vec));
                containerMutex.unlock();
                uint64_t targetTitleId = 0;
                for (auto const& x : vec) {
                    if(x.second->button == dragTarget) {
                        targetTitleId = x.first;
                    }
                }

                if(targetTitleId > 0) {
                    for(uint32_t i = 0; i< position.size(); i++) {
                        if(position[i] == targetTitleId) {
                            position[i] = currentlyHeldTitleId;
                        }
                    }
                }

                if(currentlyHeldPosition >= 0 && currentlyHeldPosition <= (int32_t) position.size()) {
                    position[currentlyHeldPosition] = targetTitleId;
                }
                dragTarget = NULL;
            } else {
                if(currentlyHeldPosition >= 0 && currentlyHeldPosition <= (int32_t) position.size()) {
                    position[currentlyHeldPosition] = currentlyHeldTitleId;
                }
            }
            currentlyHeld = NULL;
            currentlyHeldTitleId = 0;

            currentlyHeldPosition = -1;
            bUpdatePositions = true;
        } else {
            //DEBUG_FUNCTION_LINE("Holding it\n");
            bUpdatePositions = true;
        }
    }

    if(currentLeftPosition < targetLeftPosition) {
        currentLeftPosition += 35;

        if(currentLeftPosition > targetLeftPosition)
            currentLeftPosition = targetLeftPosition;

        bUpdatePositions = true;
    } else if(currentLeftPosition > targetLeftPosition) {
        currentLeftPosition -= 35;

        if(currentLeftPosition < targetLeftPosition)
            currentLeftPosition = targetLeftPosition;

        bUpdatePositions = true;
    }

    if(bUpdatePositions) {
        bUpdatePositions = false;
        updateButtonPositions();
    }
    gameLaunchTimer++;

    GuiFrame::process();
}

void GuiIconGrid::update(GuiController * c) {
    GuiFrame::update(c);
}

void GuiIconGrid::updateButtonPositions() {
    arrowRightButton.setState(GuiElement::STATE_DISABLED);
    arrowRightButton.setVisible(false);
    arrowLeftButton.setState(GuiElement::STATE_DISABLED);
    arrowLeftButton.setVisible(false);

    int32_t col = 0, row = 0, listOff = 0;

    // create an empty vector of pairs
    std::vector<std::pair<uint64_t,GameInfoContainer*>> vec;

    containerMutex.lock();

    // copy key-value pairs from the map to the vector
    std::copy(gameInfoContainers.begin(), gameInfoContainers.end(), std::back_inserter<std::vector<std::pair<uint64_t,GameInfoContainer*>>>(vec));

    containerMutex.unlock();

    for (auto const& x : vec) {
        if(x.second->button == currentlyHeld) {
            currentlyHeldTitleId = x.first;
        }
        remove(x.second->button);
    }

    for (auto const& x : emptyIcons) {
        remove(x);
    }

    if(sortByName) {
        std::sort(vec.begin(), vec.end(),
        [](const std::pair<uint64_t,GameInfoContainer*>& l, const std::pair<uint64_t,GameInfoContainer*>& r) {
            if (l.second != r.second)
                return l.second->info->name.compare(r.second->info->name) <0;

            return l.first < r.first;
        });
    }


    // TODO somehow be able to adjust the positions.

    //position.clear();
    for(uint32_t i = 0; i<position.size(); i++) {
        if(position[i] == currentlyHeldTitleId) {
            currentlyHeldPosition = i;
            position[i] = 0;
        }
    }

    uint32_t elementSize = position.size();
    uint32_t pages = (elementSize / (MAX_COLS * MAX_ROWS)) +1;

    uint32_t emptyIconUse = 0;

    if(curPage < 0) {
        curPage = 0;
    }
    if((uint32_t) curPage > pages) {
        curPage = 0;
    }

    targetLeftPosition = -curPage * getWidth();

    if((uint32_t) curPage < (pages - 1)) {
        arrowRightButton.clearState(GuiElement::STATE_DISABLED);
        arrowRightButton.setVisible(true);
        bringToFront(&arrowRightButton);
    }
    if(curPage > 0) {
        arrowLeftButton.clearState(GuiElement::STATE_DISABLED);
        arrowLeftButton.setVisible(true);
        bringToFront(&arrowLeftButton);
    }

    uint32_t startPage = -(currentLeftPosition / getWidth());
    uint32_t endPage = startPage;

    if(targetLeftPosition != currentLeftPosition) {
        for (auto const& x : vec) {
            x.second->button->setHoldable(false);
        }
        endPage++;
        if(endPage > pages) {
            endPage = pages;
        }
    }else{
        for (auto const& x : vec) {
            x.second->button->setHoldable(true);
        }
    }

    for(uint32_t i = startPage * (MAX_COLS * MAX_ROWS); i < (endPage + 1) * (MAX_COLS * MAX_ROWS); i++) {
        listOff = i / (MAX_COLS * MAX_ROWS);
        GuiElement * element = NULL;
        float posX = currentLeftPosition + listOff * width + ( col * (noIcon.getWidth() + noIcon.getWidth() * 0.5f) - (MAX_COLS * 0.5f - 0.5f) * (noIcon.getWidth() + noIcon.getWidth() * 0.5f) );
        float posY = -row * (noIcon.getHeight() + noIcon.getHeight() * 0.5f) + (MAX_ROWS * 0.5f - 0.5f) * (noIcon.getHeight() + noIcon.getHeight() * 0.5f) + 30.0f;

        if(i < position.size()) {
            uint64_t titleID = position.at(i);
            if(titleID > 0) {
                GameInfoContainer * container = NULL;
                containerMutex.lock();
                if(gameInfoContainers.find(titleID) != gameInfoContainers.end()) {
                    container = gameInfoContainers[titleID];
                }
                containerMutex.unlock();
                if(container != NULL) {
                    element = container->button;
                }
            }
        }

        if(element == NULL) {
            if(emptyIcons.size() <= emptyIconUse) {
                break;
            }
            element = emptyIcons.at(emptyIconUse);
            emptyIconUse++;
        }
        element->setPosition(posX, posY);
        append(element);

        col++;
        if(col >= MAX_COLS) {
            col = 0;
            row++;
        }
        if(row >= MAX_ROWS) {
            row = 0;
        }
    }
    if(currentlyHeld != NULL) {
        append(currentlyHeld);
    }
}

void GuiIconGrid::draw(CVideo *pVideo) {
    //! the BG needs to be rendered to stencil
    pVideo->setStencilRender(true);
    particleBgImage.draw(pVideo);
    pVideo->setStencilRender(false);

    containerMutex.lock();
    GuiFrame::draw(pVideo);
    containerMutex.unlock();
}
