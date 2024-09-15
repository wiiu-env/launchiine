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
    ~GuiDragListener() override;

    //!Set a new GuiTrigger for the element
    //!\param i Index of trigger array to set
    //!\param t Pointer to GuiTrigger
    void setTrigger(const std::shared_ptr<GuiTrigger> &, int32_t idx = -1);

    //!Constantly called to allow the GuiDragListener to respond to updated input data
    //!\param t Pointer to a GuiTrigger, containing the current input data from PAD/WPAD
    void update(const GuiController &c) override;

    sigslot::signal5<GuiDragListener *, const GuiController *, GuiTrigger *, int32_t, int32_t> dragged;

protected:
    std::array<std::shared_ptr<GuiTrigger>, 10> trigger; //!< GuiTriggers (input actions) that this element responds to
};

#endif
