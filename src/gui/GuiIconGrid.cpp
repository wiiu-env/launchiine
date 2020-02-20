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
    , touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH)
    , wpadTouchTrigger(GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5, GuiTrigger::BUTTON_A)
    , noIcon(Resources::GetFile("noGameIcon.png"), Resources::GetFileSize("noGameIcon.png"), GX2_TEX_CLAMP_MODE_MIRROR) {

    particleBgImage.setParent(this);
    selectedGame = GameIndex;
    listOffset = selectedGame / (MAX_COLS * MAX_ROWS);
    targetLeftPosition = -listOffset * getWidth();
    currentLeftPosition = targetLeftPosition;
}

GuiIconGrid::~GuiIconGrid() {
    containerMutex.lock();
    for (auto const& x : gameInfoContainers) {
        remove(x.second->button);
        delete x.second;
    }
    gameInfoContainers.clear();
    containerMutex.unlock();
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
}

uint64_t GuiIconGrid::getSelectedGame(void) {
    return selectedGame;
}

void GuiIconGrid::OnGameTitleListUpdated(GameList * gameList) {
    containerMutex.lock();
    gameList->lock();
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
    gameList->unlock();
    containerMutex.unlock();
    bUpdatePositions = true;
}

void GuiIconGrid::OnLaunchClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    //! do not auto launch when wiimote is pointing to screen and presses A
    //if((trigger == &buttonATrigger) && (controller->chan & (GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5)) && controller->data.validPointer) {
    //    return;
    //}
    DEBUG_FUNCTION_LINE("Tried to launch %s (%016llX)\n", gameInfoContainers[getSelectedGame()]->info->name.c_str(),getSelectedGame());
    gameLaunchClicked(this, getSelectedGame());
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
    if(info->imageData != NULL){
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

    GameInfoContainer * container = new GameInfoContainer(button, image, info);
    containerMutex.lock();
    gameInfoContainers[info->titleId] = container;
    containerMutex.unlock();
    this->append(button);

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

    if(container != NULL) {
        container->updateImageData();
    }
    containerMutex.unlock();
    bUpdatePositions = true;
}

void GuiIconGrid::process() {
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
    int32_t col = 0, row = 0, listOff = 0;

    int i = 0;
    // create an empty vector of pairs
    std::vector<std::pair<uint64_t,GameInfoContainer*>> vec;

    containerMutex.lock();

    // copy key-value pairs from the map to the vector
    std::copy(gameInfoContainers.begin(), gameInfoContainers.end(), std::back_inserter<std::vector<std::pair<uint64_t,GameInfoContainer*>>>(vec));

    containerMutex.unlock();

    if(sortByName) {
        std::sort(vec.begin(), vec.end(),
        [](const std::pair<uint64_t,GameInfoContainer*>& l, const std::pair<uint64_t,GameInfoContainer*>& r) {
            if (l.second != r.second)
                return l.second->info->name.compare(r.second->info->name) <0;

            return l.first < r.first;
        });
    }


    for (auto const& x : vec) {
        listOff = i / (MAX_COLS * MAX_ROWS);

        float posX = currentLeftPosition + listOff * width + ( col * (noIcon.getWidth() + noIcon.getWidth() * 0.5f) - (MAX_COLS * 0.5f - 0.5f) * (noIcon.getWidth() + noIcon.getWidth() * 0.5f) );
        float posY = -row * (noIcon.getHeight() + noIcon.getHeight() * 0.5f) + (MAX_ROWS * 0.5f - 0.5f) * (noIcon.getHeight() + noIcon.getHeight() * 0.5f) + 30.0f;

        if(x.second->button != NULL) {
            x.second->button->setPosition(posX, posY);
        }

        col++;
        if(col >= MAX_COLS) {
            col = 0;
            row++;
        }
        if(row >= MAX_ROWS)
            row = 0;

        i++;
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
