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
    GuiIconGrid(int32_t w, int32_t h, uint64_t selectedTitleId, bool sortByName);
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

    bool sortByName = false;

    GuiParticleImage particleBgImage;

    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;
    GuiTrigger leftTrigger;
    GuiTrigger rightTrigger;
    GuiTrigger downTrigger;
    GuiTrigger upTrigger;
    GuiTrigger buttonATrigger;
    GuiTrigger buttonLTrigger;
    GuiTrigger buttonRTrigger;
    GuiButton leftButton;
    GuiButton rightButton;
    GuiButton downButton;
    GuiButton upButton;
    GuiButton launchButton;

    GuiImageData* arrowRightImageData;
    GuiImageData* arrowLeftImageData;
    GuiImage arrowRightImage;
    GuiImage arrowLeftImage;
    GuiButton arrowRightButton;
    GuiButton arrowLeftButton;

    GuiImageData noIcon;

    void OnLeftArrowClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void OnRightArrowClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnLeftClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void OnRightClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void OnDownClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);
    void OnUpClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

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
        GameInfoContainer(GuiButton* button, GuiImage * image, gameInfo * info) {
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

        GuiImage * image;
        gameInfo * info;
        GuiButton * button;
    };

    std::recursive_mutex containerMutex;
    std::map<uint64_t, GameInfoContainer *> gameInfoContainers;
};
