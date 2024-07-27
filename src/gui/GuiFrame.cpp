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
#include <gui/GuiFrame.h>

GuiFrame::GuiFrame(GuiFrame *p) {
    parent = p;
    width  = 0;
    height = 0;
    dim    = false;

    if (parent) {
        parent->append(this);
    }
}

GuiFrame::GuiFrame(float w, float h, GuiFrame *p) {
    parent = p;
    width  = w;
    height = h;
    dim    = false;

    if (parent) {
        parent->append(this);
    }
}

GuiFrame::~GuiFrame() {
    closing(this);

    if (parent) {
        parent->remove(this);
    }
}

void GuiFrame::append(GuiElement *e) {
    if (e == NULL) {
        return;
    }

    remove(e);
    mutex.lock();
    elements.push_back(e);
    e->setParent(this);
    mutex.unlock();
}

void GuiFrame::insert(GuiElement *e, uint32_t index) {
    if (e == NULL || (index >= elements.size())) {
        return;
    }

    remove(e);
    mutex.lock();
    elements.insert(elements.begin() + index, e);
    e->setParent(this);
    mutex.unlock();
}

void GuiFrame::remove(GuiElement *e) {
    if (e == NULL) {
        return;
    }

    mutex.lock();
    for (uint32_t i = 0; i < elements.size(); ++i) {
        if (e == elements[i]) {
            elements.erase(elements.begin() + i);
            break;
        }
    }
    mutex.unlock();
}

void GuiFrame::removeAll() {
    mutex.lock();
    elements.clear();
    mutex.unlock();
}

void GuiFrame::close() {
    //Application::instance()->pushForDelete(this);
}

void GuiFrame::dimBackground(bool d) {
    dim = d;
}

GuiElement *GuiFrame::getGuiElementAt(uint32_t index) const {
    if (index >= elements.size()) {
        return NULL;
    }

    return elements[index];
}

uint32_t GuiFrame::getSize() {
    return elements.size();
}

void GuiFrame::resetState() {
    GuiElement::resetState();

    mutex.lock();
    for (uint32_t i = 0; i < elements.size(); ++i) {
        elements[i]->resetState();
    }
    mutex.unlock();
}

void GuiFrame::setState(int32_t s, int32_t c) {
    GuiElement::setState(s, c);
    mutex.lock();
    for (uint32_t i = 0; i < elements.size(); ++i) {
        elements[i]->setState(s, c);
    }
    mutex.unlock();
}

void GuiFrame::clearState(int32_t s, int32_t c) {
    GuiElement::clearState(s, c);

    mutex.lock();
    for (uint32_t i = 0; i < elements.size(); ++i) {
        elements[i]->clearState(s, c);
    }
    mutex.unlock();
}

void GuiFrame::setVisible(bool v) {
    visible = v;

    mutex.lock();
    for (uint32_t i = 0; i < elements.size(); ++i) {
        elements[i]->setVisible(v);
    }
    mutex.unlock();
}

int32_t GuiFrame::getSelected() {
    // find selected element
    int32_t found = -1;
    mutex.lock();
    for (uint32_t i = 0; i < elements.size(); ++i) {
        if (elements[i]->isStateSet(STATE_SELECTED | STATE_OVER)) {
            found = i;
            break;
        }
    }
    mutex.unlock();
    return found;
}

void GuiFrame::draw(CVideo *v) {
    if (!this->isVisible() && parentElement) {
        return;
    }

    if (parentElement && dim == true) {
        //GXColor dimColor = (GXColor){0, 0, 0, 0x70};
        //Menu_DrawRectangle(0, 0, GetZPosition(), screenwidth,screenheight, &dimColor, false, true);
    }

    mutex.lock();
    //! render appended items next frame but allow stop of render if size is reached
    uint32_t size = elements.size();

    for (uint32_t i = 0; i < size && i < elements.size(); ++i) {
        elements[i]->draw(v);
    }
    mutex.unlock();
}

void GuiFrame::updateEffects() {
    if (!this->isVisible() && parentElement) {
        return;
    }

    GuiElement::updateEffects();

    mutex.lock();
    //! render appended items next frame but allow stop of render if size is reached
    uint32_t size = elements.size();

    for (uint32_t i = 0; i < size && i < elements.size(); ++i) {
        elements[i]->updateEffects();
    }
    mutex.unlock();
}

void GuiFrame::process() {
    if (!this->isVisible() && parentElement) {
        return;
    }

    GuiElement::process();

    mutex.lock();
    //! render appended items next frame but allow stop of render if size is reached
    uint32_t size = elements.size();

    for (uint32_t i = 0; i < size && i < elements.size(); ++i) {
        elements[i]->process();
    }
    mutex.unlock();
}

void GuiFrame::update(GuiController *c) {
    if (isStateSet(STATE_DISABLED) && parentElement) {
        return;
    }

    mutex.lock();
    //! update appended items next frame
    uint32_t size = elements.size();

    for (uint32_t i = 0; i < size && i < elements.size(); ++i) {
        elements[i]->update(c);
    }
    mutex.unlock();
}
