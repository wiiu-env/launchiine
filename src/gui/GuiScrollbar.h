/***************************************************************************
 * Copyright (C) 2011
 * by Dimok
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any
 * purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you
 * must not claim that you wrote the original software. If you use
 * this software in a product, an acknowledgment in the product
 * documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and
 * must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 * distribution.
 ***************************************************************************/
#ifndef GUI_SCROLLBAR_HPP_
#define GUI_SCROLLBAR_HPP_

#include <gui/GuiButton.h>
#include <gui/GuiElement.h>

class GuiScrollbar : public GuiElement, public sigslot::has_slots<> {
public:
    GuiScrollbar(int32_t height);

    virtual ~GuiScrollbar();

    void ScrollOneUp();

    void ScrollOneDown();

    int32_t GetSelectedItem() {
        return SelItem;
    }

    int32_t GetSelectedIndex() {
        return SelInd;
    }

    void draw(CVideo *video) override;

    void update(GuiController *t);

    //! Signals
    sigslot::signal2<int32_t, int32_t> listChanged;

    //! Slots
    void SetScrollSpeed(int32_t speed) {
        ScrollSpeed = speed;
    };

    void SetPageSize(int32_t size);

    void SetRowSize(int32_t size);

    void SetSelectedItem(int32_t pos);

    void SetSelectedIndex(int32_t pos);

    void SetEntrieCount(int32_t cnt);

    void setSoundClick(GuiSound *snd) {
        clickSound = snd;
        arrowUpBtn->setSoundClick(snd);
        arrowDownBtn->setSoundClick(snd);
    }

    void setImageScrollbarLine(GuiImage *img) {
        if (img) {
            scrollbarLineImage = img;
            scrollbarLineImage->setParent(this);
            scrollbarLineImage->setParent(this);
            scrollbarLineImage->setAlignment(ALIGN_CENTER | ALIGN_MIDDLE);
            scrollbarLineImage->setPosition(0, 0);
        }
    }

    void setImageArrowDown(GuiImage *img) {
        if (img) {
            arrowDownImage = img;
            arrowDownBtn->setSize(img->getWidth(), img->getHeight());
            arrowDownBtn->setImage(img);
        }
    }

    void setImageArrowUp(GuiImage *img) {
        if (img) {
            arrowUpImage = img;
            arrowUpBtn->setSize(img->getWidth(), img->getHeight());
            arrowUpBtn->setImage(img);
        }
    }

    void setImageScrollbarBox(GuiImage *img) {
        if (img) {
            scrollbarBoxImage = img;
            scrollbarBoxBtn->setSize(img->getWidth(), height);
            scrollbarBoxBtn->setImage(img);

            width = img->getWidth();

            MaxHeight = height * 0.5f - (img ? (img->getHeight() * 0.5f) : 0) - (arrowUpImage ? arrowUpImage->getHeight() : 0);
            MinHeight = -height * 0.5f + (img ? (img->getHeight() * 0.5f) : 0) + (arrowDownImage ? arrowDownImage->getHeight() : 0);
        }
    }

protected:
    void setScrollboxPosition(int32_t SelItem, int32_t SelInd);

    void OnUpButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnDownButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnBoxButtonHold(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    uint32_t ScrollState;
    uint16_t ScrollSpeed;

    int32_t MinHeight;
    int32_t MaxHeight;
    int32_t SelItem;
    int32_t SelInd;
    int32_t PageSize;
    int32_t EntrieCount;
    int32_t pressedChan;

    GuiButton *arrowUpBtn;
    GuiButton *arrowDownBtn;
    GuiButton *scrollbarBoxBtn;

    GuiSound *clickSound = NULL;

    GuiImage *scrollbarLineImage = NULL;
    GuiImage *arrowDownImage     = NULL;
    GuiImage *arrowUpImage       = NULL;
    GuiImage *scrollbarBoxImage  = NULL;

    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;
};

#endif
