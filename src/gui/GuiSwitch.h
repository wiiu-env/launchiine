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
#ifndef GUI_SWTICH_H_
#define GUI_SWTICH_H_

#include <gui/GuiImage.h>
#include <gui/GuiImageData.h>
#include <gui/GuiToggle.h>

//!A simple switch
class GuiSwitch : public GuiToggle {
public:
    //!Constructor
    //!\param checked Checked
    GuiSwitch(GuiImage *background, bool checked, float w = 0.0f, float h = 0.0f);

    //!Destructor
    virtual ~GuiSwitch();

    void setImageBackground(GuiImage *img);

    void setImageOn(GuiImage *img);

    void setImageOff(GuiImage *img);

    void setImageHighlighted(GuiImage *img);

protected:
    GuiImage *backgroundImg  = NULL;
    GuiImage *onImg          = NULL;
    GuiImage *offImg         = NULL;
    GuiImage *highlightedImg = NULL;

    void draw(CVideo *v) override;
};

#endif
