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
#include <gui/GuiCheckBox.h>
#include <gui/GuiImage.h>
#include <gui/GuiImageData.h>

/**
 * Constructor for the GuiCheckBox class.
 */

GuiCheckBox::GuiCheckBox(GuiImage *background, bool checked, float width, float height)
    : GuiToggle(checked, width, height) {
    setImageBackground(background);
}

/**
 * Destructor for the GuiCheckBox class.
 */
GuiCheckBox::~GuiCheckBox() {
}

void GuiCheckBox::setImageBackground(GuiImage *img) {
    backgroundImg = img;
    if (img) {
        img->setParent(this);
    }
}

void GuiCheckBox::setImageSelected(GuiImage *img) {
    selectedImg = img;
    if (img) {
        img->setParent(this);
    }
}

void GuiCheckBox::setImageHighlighted(GuiImage *img) {
    highlightedImg = img;
    if (img) {
        img->setParent(this);
    }
    setIconOver(img);
}

void GuiCheckBox::update(GuiController *c) {
    if (bChanged) {
        if (selected) {
            GuiButton::setImage(selectedImg);
        } else {
            GuiButton::setImage(backgroundImg);
        }
        bChanged = false;
    }
    GuiToggle::update(c);
}
