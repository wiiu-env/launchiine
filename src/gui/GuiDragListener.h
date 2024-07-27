/****************************************************************************
 * Copyright (C) 2016 Maschell
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
#ifndef GUI_DRAG_LISTENER_H_
#define GUI_DRAG_LISTENER_H_

#include <gui/GuiButton.h>
#include <gui/GuiController.h>
#include <gui/GuiElement.h>
#include <gui/GuiTrigger.h>

class GuiDragListener : public GuiElement {
public:
    //!Constructor
    //!\param w Width
    //!\param h Height
    GuiDragListener(float w, float h);

    //!Destructor
    virtual ~GuiDragListener();

    void setState(int32_t i, int32_t c);

    //!Set a new GuiTrigger for the element
    //!\param i Index of trigger array to set
    //!\param t Pointer to GuiTrigger
    void setTrigger(GuiTrigger *t, int32_t idx = -1);

    //!Constantly called to allow the GuiDragListener to respond to updated input data
    //!\param t Pointer to a GuiTrigger, containing the current input data from PAD/WPAD
    void update(GuiController *c);

    sigslot::signal5<GuiDragListener *, const GuiController *, GuiTrigger *, int32_t, int32_t> dragged;

protected:
    static const int32_t iMaxGuiTriggers = 10;

    GuiTrigger *trigger[iMaxGuiTriggers]; //!< GuiTriggers (input actions) that this element responds to
};

#endif
