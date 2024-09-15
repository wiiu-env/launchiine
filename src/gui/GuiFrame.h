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
#ifndef GUI_FRAME_H_
#define GUI_FRAME_H_

#include <gui/GuiElement.h>
#include <gui/sigslot.h>
#include <memory>
#include <mutex>
#include <vector>

//!Allows GuiElements to be grouped together into a "window"
class GuiFrame : public GuiElement {
public:
    //!Constructor
    GuiFrame();

    //!\overload
    //!\param w Width of window
    //!\param h Height of window
    GuiFrame(float w, float h);

    //!Destructor
    ~GuiFrame() override;

    //!Appends a GuiElement to the GuiFrame
    //!\param e The GuiElement to append. If it is already in the GuiFrame, it is removed first
    void append(std::unique_ptr<GuiElement> e);

    //!Inserts a GuiElement into the GuiFrame at the specified index
    //!\param e The GuiElement to insert. If it is already in the GuiFrame, it is removed first
    //!\param i Index in which to insert the element
    void insert(std::unique_ptr<GuiElement>, uint32_t i);



    //!Removes all GuiElements
    void removeAll();

    //!Bring element to front of the window
    void bringToFront(GuiElement *e) {
        remove(e);
        append(e);
    }


    //!Sets the visibility of the window
    //!\param v visibility (true = visible)
    void setVisible(bool v) override;

    //!Resets the window's state to STATE_DEFAULT
    void resetState() override;

    //!Sets the window's state
    //!\param s State
    void setState(int32_t s, int32_t c = -1) override;

    void clearState(int32_t s, int32_t c = -1) override;

    //!Gets the index of the GuiElement inside the window that is currently selected
    //!\return index of selected GuiElement
    int32_t getSelected() override;

    //!Dim the Window's background
    void dimBackground(bool d);

    //!Draws all the elements in this GuiFrame
    void draw(const CVideo& v) override;

    //!Updates the window and all elements contains within
    //!Allows the GuiFrame and all elements to respond to the input data specified
    //!\param t Pointer to a GuiTrigger, containing the current input data from PAD/WPAD
    void update(const GuiController& t);

    //!virtual updateEffects which is called by the main loop
    void updateEffects() override;

    //!virtual process which is called by the main loop
    void process() override;

    //! Signals
    //! On Closing
    sigslot::signal1<GuiFrame *> closing;

protected:
    //!Removes the specified GuiElement from the GuiFrame
    void remove(GuiElement *e);

    bool dim;                                          //! Enable/disable dim of a window only
    GuiFrame *parent;                                  //!< Parent Window
    std::vector<std::unique_ptr<GuiElement>> elements; //!< Contains all elements within the GuiFrame
    std::recursive_mutex mutex;

};

#endif
