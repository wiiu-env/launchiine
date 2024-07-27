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
#include "utils/utils.h"
#include <gui/GuiScrollbar.h>

GuiScrollbar::GuiScrollbar(int32_t h)
    : touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH), wpadTouchTrigger(GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5, GuiTrigger::BUTTON_A) {
    SelItem     = 0;
    SelInd      = 0;
    PageSize    = 0;
    EntrieCount = 0;
    SetScrollSpeed(15);
    ScrollState = 0;

    listChanged.connect(this, &GuiScrollbar::setScrollboxPosition);

    height = h;

    arrowUpBtn = new GuiButton(50, 50);
    arrowUpBtn->setParent(this);
    arrowUpBtn->setAlignment(ALIGN_CENTER | ALIGN_TOP);
    arrowUpBtn->setPosition(0, 0);
    arrowUpBtn->setTrigger(&touchTrigger, 0);
    arrowUpBtn->setTrigger(&wpadTouchTrigger, 1);
    arrowUpBtn->setEffectGrow();
    arrowUpBtn->clicked.connect(this, &GuiScrollbar::OnUpButtonClick);

    arrowDownBtn = new GuiButton(50, 50);
    arrowDownBtn->setParent(this);
    arrowDownBtn->setAlignment(ALIGN_CENTER | ALIGN_BOTTOM);
    arrowDownBtn->setPosition(0, 0);
    arrowDownBtn->setTrigger(&touchTrigger, 0);
    arrowDownBtn->setTrigger(&wpadTouchTrigger, 1);
    arrowDownBtn->setEffectGrow();
    arrowDownBtn->clicked.connect(this, &GuiScrollbar::OnDownButtonClick);

    scrollbarBoxBtn = new GuiButton(50, height);
    scrollbarBoxBtn->setParent(this);
    scrollbarBoxBtn->setAlignment(ALIGN_CENTER | ALIGN_TOP);
    scrollbarBoxBtn->setPosition(0, MaxHeight);
    scrollbarBoxBtn->setHoldable(true);
    scrollbarBoxBtn->setTrigger(&touchTrigger, 0);
    scrollbarBoxBtn->setTrigger(&wpadTouchTrigger, 1);
    scrollbarBoxBtn->setEffectGrow();
    scrollbarBoxBtn->held.connect(this, &GuiScrollbar::OnBoxButtonHold);
}

GuiScrollbar::~GuiScrollbar() {
    delete arrowUpBtn;
    delete arrowDownBtn;
    delete scrollbarBoxBtn;
}

void GuiScrollbar::ScrollOneUp() {
    if (SelItem == 0 && SelInd > 0) {
        // move list up by 1
        --SelInd;
    } else if (SelInd + SelItem > 0) {
        --SelItem;
    }
}

void GuiScrollbar::ScrollOneDown() {
    if (SelInd + SelItem + 1 < EntrieCount) {
        if (SelItem == PageSize - 1) {
            // move list down by 1
            SelInd++;
        } else {
            SelItem++;
        }
    }
}

void GuiScrollbar::OnUpButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    if (ScrollState < ScrollSpeed) {
        return;
    }

    ScrollOneUp();

    ScrollState = 0;
    listChanged(SelItem, SelInd);
}

void GuiScrollbar::OnDownButtonClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    if (ScrollState < ScrollSpeed) {
        return;
    }

    ScrollOneDown();

    ScrollState = 0;
    listChanged(SelItem, SelInd);
}

void GuiScrollbar::OnBoxButtonHold(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    if (EntrieCount == 0) {
        return;
    }

    if (!controller->data.validPointer) {
        return;
    }

    int32_t y = controller->data.y - this->getCenterY();

    int32_t positionWiimote = LIMIT(y - MinHeight, 0, MaxHeight - MinHeight);

    int32_t newSelected = (EntrieCount - 1) - (int32_t) ((float) positionWiimote / (float) (MaxHeight - MinHeight) * (float) (EntrieCount - 1));

    int32_t diff = newSelected - SelInd - SelItem;

    if (newSelected <= 0) {
        SelItem = 0;
        SelInd  = 0;
    } else if (newSelected >= EntrieCount - 1) {
        SelItem = (PageSize - 1 < EntrieCount - 1) ? PageSize - 1 : EntrieCount - 1;
        SelInd  = EntrieCount - PageSize;
    } else if (newSelected < PageSize && SelInd == 0 && diff < 0) {
        SelItem = std::max(SelItem + diff, (int32_t) 0);
    } else if (EntrieCount - newSelected < PageSize && SelInd == EntrieCount - PageSize && diff > 0) {
        SelItem = std::min(SelItem + diff, PageSize - 1);
    } else {
        SelInd = LIMIT(SelInd + diff, 0, ((EntrieCount - PageSize < 0) ? 0 : EntrieCount - PageSize));
    }

    ScrollState = 0;
    listChanged(SelItem, SelInd);
}

void GuiScrollbar::SetPageSize(int32_t size) {
    if (PageSize == size) {
        return;
    }

    PageSize = size;
    listChanged(SelItem, SelInd);
}

void GuiScrollbar::SetSelectedItem(int32_t pos) {
    if (SelItem == pos) {
        return;
    }

    SelItem = LIMIT(pos, 0, EntrieCount - 1);
    listChanged(SelItem, SelInd);
}

void GuiScrollbar::SetSelectedIndex(int32_t pos) {
    if (SelInd == pos) {
        return;
    }

    SelInd = pos;
    listChanged(SelItem, SelInd);
}

void GuiScrollbar::SetEntrieCount(int32_t cnt) {
    if (EntrieCount == cnt) {
        return;
    }

    EntrieCount = cnt;
    listChanged(SelItem, SelInd);
}

void GuiScrollbar::setScrollboxPosition(int32_t SelItem, int32_t SelInd) {
    int32_t position = MaxHeight - (MaxHeight - MinHeight) * (SelInd + SelItem) / (EntrieCount - 1);

    if (position < MinHeight || (SelInd + SelItem >= EntrieCount - 1)) {
        position = MinHeight;
    } else if (position > MaxHeight || (SelInd + SelItem) == 0) {
        position = MaxHeight;
    }

    scrollbarBoxBtn->setPosition(0, position);
}

void GuiScrollbar::draw(CVideo *video) {
    if (scrollbarLineImage) {
        scrollbarLineImage->draw(video);
    }
    arrowUpBtn->draw(video);
    arrowDownBtn->draw(video);
    scrollbarBoxBtn->draw(video);

    updateEffects();
}

void GuiScrollbar::update(GuiController *t) {
    if (this->isStateSet(STATE_DISABLED)) {
        return;
    }

    arrowUpBtn->update(t);
    arrowDownBtn->update(t);
    scrollbarBoxBtn->update(t);

    ++ScrollState;
}
