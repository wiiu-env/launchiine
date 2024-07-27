/****************************************************************************
 * Copyright (C) 2016,2017 Maschell
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
#include <gui/GuiImage.h>
#include <gui/GuiImageData.h>
#include <gui/GuiSwitch.h>

/**
 * Constructor for the GuiSwitch class.
 */

GuiSwitch::GuiSwitch(GuiImage *background, bool checked, float w, float h)
    : GuiToggle(checked, w, h) {
    setImageBackground(background);
}

/**
 * Destructor for the GuiSwitch class.
 */
GuiSwitch::~GuiSwitch() {
}

void GuiSwitch::setImageBackground(GuiImage *img) {
    backgroundImg = img;
    if (img) {
        img->setParent(this);
    }
    setImage(img);
}

void GuiSwitch::setImageOn(GuiImage *img) {
    onImg = img;
    if (img) {
        img->setParent(this);
        img->setAlignment(ALIGN_RIGHT);
    }
}

void GuiSwitch::setImageOff(GuiImage *img) {
    offImg = img;
    if (img) {
        img->setParent(this);
        img->setAlignment(ALIGN_LEFT);
    }
}

void GuiSwitch::setImageHighlighted(GuiImage *img) {
    highlightedImg = img;
    setIconOver(img);
}

void GuiSwitch::draw(CVideo *v) {
    if (!this->isVisible()) {
        return;
    }

    GuiToggle::draw(v);
    if (getValue()) {
        if (onImg != NULL) {
            onImg->draw(v);
        }
    } else {
        if (offImg != NULL) {
            offImg->draw(v);
        }
    }
}
