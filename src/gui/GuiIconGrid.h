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
#pragma once

#include <map>
#include "gui/GuiTitleBrowser.h"
#include "gui/GameIcon.h"
#include <gui/GuiParticleImage.h>
#include "utils/AsyncExecutor.h"
#include "utils/logger.h"

class GuiIconGrid : public GuiTitleBrowser, public sigslot::has_slots<> {
public:
    GuiIconGrid(int32_t w, int32_t h, uint64_t selectedTitleId);
    virtual ~GuiIconGrid();

    void setSelectedGame(uint64_t idx);
    uint64_t getSelectedGame(void);

    void update(GuiController * t);
    void draw(CVideo *pVideo);
    void process();

    void OnGameTitleListUpdated(GameList * list);
    void OnAddGameTitle(gameInfo * info);
    void OnGameTitleUpdated(gameInfo * info);
    void OnGameTitleAdded(gameInfo * info);
private:
    static const int32_t MAX_ROWS = 3;
    static const int32_t MAX_COLS = 5;

    GuiSound *buttonClickSound;

    GuiParticleImage particleBgImage;

    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;

    GuiImageData noIcon;

    void OnLaunchClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void OnGameButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void updateButtonPositions();

    int32_t listOffset;
    uint64_t selectedGame;
    int32_t currentLeftPosition;
    int32_t targetLeftPosition;
    uint32_t gameLaunchTimer;
    bool bUpdatePositions = false;

    class GameInfoContainer {
    public:
        GameInfoContainer(GuiButton* button, GameIcon * image, gameInfo * info) {
            this->image = image;
            this->info = info;
            this->button = button;
        }

        ~GameInfoContainer() {
            if(button != NULL) {
                AsyncExecutor::pushForDelete(button);
            }
            if(image != NULL) {
                AsyncExecutor::pushForDelete(image);
            }
        }

        void updateImageData() {
            if(image != NULL && info != NULL && info->imageData != NULL) {
                image->setImageData(info->imageData);
            }
        }

        GameIcon * image;
        gameInfo * info;
        GuiButton * button;
    };

    CMutex containerMutex;
    std::map<uint64_t, GameInfoContainer *> gameInfoContainers;
};
