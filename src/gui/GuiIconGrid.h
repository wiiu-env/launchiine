#pragma once

#include "gui/GameIcon.h"
#include "gui/GuiDragListener.h"
#include "gui/GuiTitleBrowser.h"
#include "utils/AsyncExecutor.h"
#include "utils/logger.h"
#include <gui/GuiParticleImage.h>
#include <map>

class GuiIconGrid : public GuiTitleBrowser, public sigslot::has_slots<> {
public:
    GuiIconGrid(int32_t w, int32_t h, uint64_t selectedTitleId, bool sortByName);

    virtual ~GuiIconGrid();

    void setSelectedGame(uint64_t idx);

    uint64_t getSelectedGame(void);

    void update(GuiController *t);

    void draw(CVideo *pVideo);

    void process();

    void OnGameTitleListUpdated(GameList *list);

    void OnAddGameTitle(gameInfo *info);

    void OnGameTitleUpdated(gameInfo *info);

    void OnGameTitleAdded(gameInfo *info);

private:
    static const int32_t MAX_ROWS = 3;
    static const int32_t MAX_COLS = 5;

    bool sortByName = false;

    GuiParticleImage particleBgImage;

    GuiSound *buttonClickSound;

    GuiText gameTitle;
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

    GuiImageData *arrowRightImageData;
    GuiImageData *arrowLeftImageData;
    GuiImage arrowRightImage;
    GuiImage arrowLeftImage;
    GuiButton arrowRightButton;
    GuiButton arrowLeftButton;

    GuiImageData noIcon;
    GuiImageData emptyIcon;

    GuiDragListener dragListener;

    void OnLeftArrowClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnRightArrowClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnLeftClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnRightClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnDownClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnUpClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnLaunchClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnGameButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnGameButtonHeld(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnGameButtonPointedOn(GuiButton *button, const GuiController *controller);

    void OnGameButtonPointedOff(GuiButton *button, const GuiController *controller);

    void OnDrag(GuiDragListener *button, const GuiController *controller, GuiTrigger *trigger, int32_t dx, int32_t dy);

    void OnLeftArrowHeld(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnRightArrowHeld(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnLeftArrowReleased(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnRightArrowReleased(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void updateButtonPositions();

    int32_t offsetForTitleId(uint64_t titleId);

    uint32_t lArrowHeldCounter = 0;
    uint32_t rArrowHeldCounter = 0;

    int32_t curPage = 0;
    int32_t listOffset;
    uint64_t selectedGame;
    int32_t currentLeftPosition;
    int32_t targetLeftPosition;
    uint32_t gameLaunchTimer;
    bool bUpdatePositions         = false;
    GuiButton *currentlyHeld      = nullptr;
    uint64_t currentlyHeldTitleId = 0;
    int32_t currentlyHeldPosition = -1;
    GuiButton *dragTarget         = nullptr;

    class GameInfoContainer {
    public:
        GameInfoContainer(GuiButton *button, GameIcon *image, gameInfo *info) {
            this->image  = image;
            this->info   = info;
            this->button = button;
        }

        ~GameInfoContainer() {
            if (button != nullptr) {
                AsyncExecutor::pushForDelete(button);
            }
            if (image != nullptr) {
                AsyncExecutor::pushForDelete(image);
            }
        }

        void updateImageData() {
            if (image != nullptr && info != nullptr && info->imageData != nullptr) {
                image->setImageData(info->imageData);
            }
        }

        GameIcon *image;
        gameInfo *info;
        GuiButton *button;
    };

    std::recursive_mutex positionMutex;
    std::recursive_mutex containerMutex;
    std::map<uint64_t, GameInfoContainer *> gameInfoContainers;
    std::vector<uint64_t> position;
    std::vector<GuiButton *> positionButtons;

    std::vector<GuiImage *> emptyIcons;
    std::vector<GuiButton *> emptyButtons;
};
