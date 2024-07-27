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
#include <gui/GuiButton.h>
#include <gui/GuiController.h>
#include <gui/GuiTrigger.h>

/**
 * Constructor for the GuiButton class.
 */

GuiButton::GuiButton(float w, float h) {
    width      = w;
    height     = h;
    image      = NULL;
    imageOver  = NULL;
    imageHold  = NULL;
    imageClick = NULL;
    icon       = NULL;
    iconOver   = NULL;

    for (int32_t i = 0; i < 4; i++) {
        label[i]      = NULL;
        labelOver[i]  = NULL;
        labelHold[i]  = NULL;
        labelClick[i] = NULL;
    }
    for (int32_t i = 0; i < iMaxGuiTriggers; i++) {
        trigger[i] = NULL;
    }

    soundOver      = NULL;
    soundHold      = NULL;
    soundClick     = NULL;
    clickedTrigger = NULL;
    heldTrigger    = NULL;
    selectable     = true;
    holdable       = false;
    clickable      = true;
}

/**
 * Destructor for the GuiButton class.
 */
GuiButton::~GuiButton() {
}

void GuiButton::setImage(GuiImage *img) {
    image = img;
    if (img) { img->setParent(this); }
}

void GuiButton::setImageOver(GuiImage *img) {
    imageOver = img;
    if (img) { img->setParent(this); }
}

void GuiButton::setImageHold(GuiImage *img) {
    imageHold = img;
    if (img) { img->setParent(this); }
}

void GuiButton::setImageClick(GuiImage *img) {
    imageClick = img;
    if (img) { img->setParent(this); }
}

void GuiButton::setIcon(GuiImage *img) {
    icon = img;
    if (img) { img->setParent(this); }
}

void GuiButton::setIconOver(GuiImage *img) {
    iconOver = img;
    if (img) { img->setParent(this); }
}

void GuiButton::setLabel(GuiText *txt, int32_t n) {
    label[n] = txt;
    if (txt) { txt->setParent(this); }
}

void GuiButton::setLabelOver(GuiText *txt, int32_t n) {
    labelOver[n] = txt;
    if (txt) { txt->setParent(this); }
}

void GuiButton::setLabelHold(GuiText *txt, int32_t n) {
    labelHold[n] = txt;
    if (txt) { txt->setParent(this); }
}

void GuiButton::setLabelClick(GuiText *txt, int32_t n) {
    labelClick[n] = txt;
    if (txt) { txt->setParent(this); }
}

void GuiButton::setSoundOver(GuiSound *snd) {
    soundOver = snd;
}

void GuiButton::setSoundHold(GuiSound *snd) {
    soundHold = snd;
}

void GuiButton::setSoundClick(GuiSound *snd) {
    soundClick = snd;
}

void GuiButton::setTrigger(GuiTrigger *t, int32_t idx) {
    if (idx >= 0 && idx < iMaxGuiTriggers) {
        trigger[idx] = t;
    } else {
        for (int32_t i = 0; i < iMaxGuiTriggers; i++) {
            if (!trigger[i]) {
                trigger[i] = t;
                break;
            }
        }
    }
}

void GuiButton::resetState(void) {
    clickedTrigger = NULL;
    heldTrigger    = NULL;
    GuiElement::resetState();
}

/**
 * Draw the button on screen
 */
void GuiButton::draw(CVideo *v) {
    if (!this->isVisible()) {
        return;
    }

    // draw image
    if ((isDrawOverOnlyWhenSelected() && (isStateSet(STATE_SELECTED) && imageOver)) ||
        (!isDrawOverOnlyWhenSelected() && (isStateSet(STATE_OVER | STATE_SELECTED | STATE_CLICKED | STATE_HELD) && imageOver))) {
        imageOver->draw(v);
    } else if (image) {
        image->draw(v);
    }

    if ((isDrawOverOnlyWhenSelected() && (isStateSet(STATE_SELECTED) && iconOver)) ||
        (!isDrawOverOnlyWhenSelected() && (isStateSet(STATE_OVER | STATE_SELECTED | STATE_CLICKED | STATE_HELD) && iconOver))) {
        iconOver->draw(v);
    } else if (icon) {
        icon->draw(v);
    }

    // draw text
    for (int32_t i = 0; i < 4; i++) {
        if (isStateSet(STATE_OVER | STATE_SELECTED | STATE_CLICKED | STATE_HELD) && labelOver[i]) {
            labelOver[i]->draw(v);
        } else if (label[i]) {
            label[i]->draw(v);
        }
    }
}

void GuiButton::update(GuiController *c) {
    if (!c || isStateSet(STATE_DISABLED | STATE_HIDDEN | STATE_DISABLE_INPUT, c->chanIdx)) {
        return;
    } else if (parentElement && (parentElement->isStateSet(STATE_DISABLED | STATE_HIDDEN | STATE_DISABLE_INPUT, c->chanIdx))) {
        return;
    }

    if (selectable) {
        if (c->data.validPointer && this->isInside(c->data.x, c->data.y)) {
            if (!isStateSet(STATE_OVER, c->chanIdx)) {
                setState(STATE_OVER, c->chanIdx);

                //if(this->isRumbleActive())
                //	this->rumble(t->chan);

                if (soundOver) {
                    soundOver->Play();
                }

                if (effectsOver && !effects) {
                    // initiate effects
                    effects      = effectsOver;
                    effectAmount = effectAmountOver;
                    effectTarget = effectTargetOver;
                }

                pointedOn(this, c);
            }
        } else if (isStateSet(STATE_OVER, c->chanIdx)) {
            this->clearState(STATE_OVER, c->chanIdx);
            pointedOff(this, c);

            if (effectTarget == effectTargetOver && effectAmount == effectAmountOver) {
                // initiate effects (in reverse)
                effects      = effectsOver;
                effectAmount = -effectAmountOver;
                effectTarget = 100;
            }
        }
    }

    for (int32_t i = 0; i < iMaxGuiTriggers; i++) {
        if (!trigger[i]) {
            continue;
        }

        // button triggers
        if (clickable) {

            int32_t isClicked = trigger[i]->clicked(c);

            if (!clickedTrigger && (isClicked != GuiTrigger::CLICKED_NONE) && (trigger[i]->isClickEverywhere() || (isStateSet(STATE_SELECTED | STATE_OVER, c->chanIdx) && trigger[i]->isSelectionClickEverywhere()) || this->isInside(c->data.x, c->data.y))) {
                if (soundClick) {
                    soundClick->Play();
                }

                clickedTrigger = trigger[i];

                if (!isStateSet(STATE_CLICKED, c->chanIdx)) {
                    if (isClicked == GuiTrigger::CLICKED_TOUCH) {
                        setState(STATE_CLICKED_TOUCH, c->chanIdx);
                    } else {
                        setState(STATE_CLICKED, c->chanIdx);
                    }
                }

                clicked(this, c, trigger[i]);
            } else if ((isStateSet(STATE_CLICKED, c->chanIdx) || isStateSet(STATE_CLICKED_TOUCH, c->chanIdx)) && (clickedTrigger == trigger[i]) && !isStateSet(STATE_HELD, c->chanIdx) && !trigger[i]->held(c) &&
                       ((isClicked == GuiTrigger::CLICKED_NONE) || trigger[i]->released(c))) {
                if ((isStateSet(STATE_CLICKED_TOUCH, c->chanIdx) && this->isInside(c->data.x, c->data.y)) || (isStateSet(STATE_CLICKED, c->chanIdx))) {
                    clickedTrigger = NULL;
                    clearState(STATE_CLICKED, c->chanIdx);
                    released(this, c, trigger[i]);
                }
            }
        }

        if (holdable) {
            bool isHeld = trigger[i]->held(c);

            if ((!heldTrigger || heldTrigger == trigger[i]) && isHeld && (trigger[i]->isHoldEverywhere() || (isStateSet(STATE_SELECTED | STATE_OVER, c->chanIdx) && trigger[i]->isSelectionClickEverywhere()) || this->isInside(c->data.x, c->data.y))) {
                heldTrigger = trigger[i];

                if (!isStateSet(STATE_HELD, c->chanIdx)) {
                    setState(STATE_HELD, c->chanIdx);
                }

                held(this, c, trigger[i]);
            } else if (isStateSet(STATE_HELD, c->chanIdx) && (heldTrigger == trigger[i]) && (!isHeld || trigger[i]->released(c))) {
                //! click is removed at this point and converted to held
                if (clickedTrigger == trigger[i]) {
                    clickedTrigger = NULL;
                    clearState(STATE_CLICKED, c->chanIdx);
                }
                heldTrigger = NULL;
                clearState(STATE_HELD, c->chanIdx);
                released(this, c, trigger[i]);
            }
        }
    }
}
