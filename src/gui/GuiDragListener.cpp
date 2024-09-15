/****************************************************************************
 * Copyright (C) 2016 Maschell
 * based on GuiButton by dimok
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
#include <gui/GuiController.h>
#include <gui/GuiDragListener.h>

/**
 * Constructor for the GuiDragListener class.
 */

GuiDragListener::GuiDragListener(float w, float h) {
    width  = w;
    height = h;
    for (auto &i : trigger) {
        i.reset();
    }
}

/**
 * Destructor for the GuiDragListener class.
 */
GuiDragListener::~GuiDragListener() = default;


void GuiDragListener::setTrigger(const std::shared_ptr<GuiTrigger> &t, int32_t idx) {
    if (idx >= 0 && idx < trigger.size()) {
        trigger[idx] = t;
    } else {
        for (auto &i : trigger) {
            if (!i) {
                i = t;
                break;
            }
        }
    }
}

void GuiDragListener::update(const GuiController &c) {
    if (isStateSet(STATE_DISABLED | STATE_HIDDEN | STATE_DISABLE_INPUT, c.chanIdx)) {
        return;
    } else if (parentElement && (parentElement->isStateSet(STATE_DISABLED | STATE_HIDDEN | STATE_DISABLE_INPUT, c.chanIdx))) {
        return;
    }

    for (auto &i : trigger) {
        if (!i) {
            continue;
        }

        bool isHeld = i->held(c);

        if (isHeld && this->isInside(c.data.x, c.data.y)) {
            int32_t dx = c.data.x - c.lastData.x;
            int32_t dy = c.data.y - c.lastData.y;

            if (dx == 0 && dy == 0) { continue; }

            dragged(this, &c, i.get(), dx, dy);
        }
    }
}
