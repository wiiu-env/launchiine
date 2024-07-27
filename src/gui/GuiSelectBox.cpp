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
#include <string>
#include <vector>

#include <gui/GuiImage.h>
#include <gui/GuiImageData.h>
#include <gui/GuiSelectBox.h>
#include <gui/GuiTrigger.h>

/**
 * Constructor for the GuiCheckBox class.
 */

GuiSelectBox::GuiSelectBox(GuiImage *background, std::string caption, float width, float height, GuiFrame *parent)
    : GuiFrame(width, height, parent), selected(0), captionText(caption), topValueButton(0, 0), touchTrigger(GuiTrigger::CHANNEL_1, GuiTrigger::VPAD_TOUCH),
      wpadTouchTrigger(GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5, GuiTrigger::BUTTON_A), buttonATrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_A, true),
      buttonBTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_B, true), buttonUpTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_UP | GuiTrigger::STICK_L_UP, true),
      buttonDownTrigger(GuiTrigger::CHANNEL_ALL, GuiTrigger::BUTTON_DOWN | GuiTrigger::STICK_L_DOWN, true), DPADButtons(0, 0) {
    setImageTopBackground(background);
    showValues       = false;
    bChanged         = false;
    bSelectedChanged = false;
    opened           = false;
    topValueText.setFontSize(32);
    topValueText.setAlignment(ALIGN_LEFT);
    topValueText.setPosition(10, -7);
    topValueButton.setLabel(&topValueText);
    topValueButton.setTrigger(&touchTrigger);
    topValueButton.setTrigger(&wpadTouchTrigger);
    topValueButton.clicked.connect(this, &GuiSelectBox::OnTopValueClicked);

    valuesFrame.setState(STATE_HIDDEN);

    DPADButtons.setTrigger(&buttonBTrigger);
    DPADButtons.setTrigger(&buttonATrigger);
    DPADButtons.setTrigger(&buttonDownTrigger);
    DPADButtons.setTrigger(&buttonUpTrigger);
    DPADButtons.clicked.connect(this, &GuiSelectBox::OnDPADClick);
    DPADButtons.setState(STATE_DISABLE_INPUT);

    append(&DPADButtons);
    append(&valuesFrame);
    append(&topValueButton);

    showValues = false;
    bChanged   = true;
}

void GuiSelectBox::setSize(float width, float height) {
    GuiFrame::setSize(width, height);
    topValueButton.setSize(width, height);
}

void GuiSelectBox::OnValueClicked(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    for (uint32_t i = 0; i < valueButtons.size(); ++i) {
        if (valueButtons[i].valueButton == button) {
            selected = i;
            SelectValue(i);
            break;
        }
    }
}

void GuiSelectBox::SelectValue(uint32_t value) {
    if (value < valueButtons.size()) {
        const wchar_t *w_text = valueButtons[value].valueButtonText->getText();
        std::wstring ws(w_text);
        std::string text(ws.begin(), ws.end());
        topValueText.setText(text.c_str());

        std::string real_value = buttonToValue[valueButtons[value].valueButton];
        if (real_value.compare(std::string()) == 0) { real_value = "<error>"; }

        valueChanged(this, real_value);
        ShowHideValues(false);
    }
}

void GuiSelectBox::OnTopValueClicked(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    ShowHideValues(!showValues);
}

void GuiSelectBox::ShowHideValues(bool showhide) {
    showValues = showhide;
    bChanged   = true;
}

void GuiSelectBox::OnDPADClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger) {
    if (opened == true) {
        if (trigger == &buttonATrigger) {
            //! do not auto launch when wiimote is pointing to screen and presses A
            if ((controller->chan & (GuiTrigger::CHANNEL_2 | GuiTrigger::CHANNEL_3 | GuiTrigger::CHANNEL_4 | GuiTrigger::CHANNEL_5)) && controller->data.validPointer) {
                return;
            }
            SelectValue(selected);
        } else if (trigger == &buttonBTrigger) {
            if (button == &DPADButtons) {
                ShowHideValues(false);
            } else {
            }
        } else if (trigger == &buttonUpTrigger) {
            if (selected > 0) { selected--; }
            bSelectedChanged = true;
        } else if (trigger == &buttonDownTrigger) {
            selected++;
            if (selected >= valueButtons.size()) { selected = valueButtons.size() - 1; }
            bSelectedChanged = true;
        }
    }
}

void GuiSelectBox::Init(std::map<std::string, std::string> values, int32_t valueID) {
    if ((uint32_t) valueID >= values.size()) {
        valueID = 0;
    }

    selected         = valueID;
    bSelectedChanged = true;

    DeleteValueData();

    if (valueImageData == NULL || valueSelectedImageData == NULL || valueHighlightedImageData == NULL) {
        return;
    }

    valueButtons.resize(values.size());

    int32_t i      = 0;
    float imgScale = 1.0f;
    std::map<std::string, std::string>::iterator itr;
    for (itr = values.begin(); itr != values.end(); itr++) {
        if (i == valueID) {
            topValueText.setText(itr->first.c_str());
        }

        valueButtons[i].valueButtonImg            = new GuiImage(valueImageData);
        valueButtons[i].valueButtonCheckedImg     = new GuiImage(valueSelectedImageData);
        valueButtons[i].valueButtonHighlightedImg = new GuiImage(valueHighlightedImageData);

        valueButtons[i].valueButton     = new GuiButton(valueButtons[i].valueButtonImg->getWidth() * imgScale, valueButtons[i].valueButtonImg->getHeight() * imgScale);
        valueButtons[i].valueButtonText = new GuiText(itr->first.c_str(), 32, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        valueButtons[i].valueButtonText->setMaxWidth(valueButtons[i].valueButtonImg->getWidth() * imgScale - 20.0f, GuiText::WRAP);
        valueButtons[i].valueButtonText->setPosition(0, 0);

        valueButtons[i].valueButtonImg->setScale(imgScale);
        valueButtons[i].valueButtonCheckedImg->setScale(imgScale);

        valueButtons[i].valueButton->setImage(valueButtons[i].valueButtonImg);
        valueButtons[i].valueButton->setIconOver(valueButtons[i].valueButtonHighlightedImg);
        valueButtons[i].valueButton->setTrigger(&touchTrigger);
        valueButtons[i].valueButton->setTrigger(&wpadTouchTrigger);
        valueButtons[i].valueButton->clicked.connect(this, &GuiSelectBox::OnValueClicked);
        valueButtons[i].valueButton->setSoundClick(buttonClickSound);
        valueButtons[i].valueButton->setLabel(valueButtons[i].valueButtonText);

        //valueButtons[i].valueButton->setState(STATE_HIDDEN); //Wont get disabled soon enough

        buttonToValue[valueButtons[i].valueButton] = itr->second;

        float topHeight = 0;
        if (topBackgroundImg != NULL) {
            topHeight = topBackgroundImg->getHeight();
        }

        int32_t ypos = (((valueButtons[i].valueButtonImg->getHeight() * getScale()) * (i)) + (topHeight - 5) * getScale()) * -1.0f;
        valueButtons[i].valueButton->setPosition(0, ypos);
        valuesFrame.append(valueButtons[i].valueButton);

        i++;
    }

    //Collapse the thing!
    showValues = false;
    bChanged   = true;
}

void GuiSelectBox::DeleteValueData() {
    for (uint32_t i = 0; i < valueButtons.size(); ++i) {
        valuesFrame.remove(valueButtons[i].valueButton);
        delete valueButtons[i].valueButtonImg;
        delete valueButtons[i].valueButtonCheckedImg;
        delete valueButtons[i].valueButtonHighlightedImg;
        delete valueButtons[i].valueButton;
        delete valueButtons[i].valueButtonText;
    }
    buttonToValue.clear();
    valueButtons.clear();
}

/**
 * Destructor for the GuiButton class.
 */
GuiSelectBox::~GuiSelectBox() {
    DeleteValueData();
    bChanged   = false;
    selected   = 0;
    showValues = false;
}


void GuiSelectBox::setState(int32_t s, int32_t c) {
    GuiElement::setState(s, c);
}

void GuiSelectBox::OnValueCloseEffectFinish(GuiElement *element) {
    valuesFrame.effectFinished.disconnect(this);
}

float GuiSelectBox::getTopValueHeight() {
    if (topBackgroundImg == NULL) {
        return 0.0f;
    }
    return topBackgroundImg->getHeight();
}

float GuiSelectBox::getTopValueWidth() {
    if (topBackgroundImg == NULL) {
        return 0.0f;
    }
    return topBackgroundImg->getWidth();
}

float GuiSelectBox::getHeight() {
    return getTopValueHeight();
}

float GuiSelectBox::getWidth() {
    return getTopValueWidth();
}


void GuiSelectBox::OnValueOpenEffectFinish(GuiElement *element) {
    valuesFrame.effectFinished.disconnect(this);
    opened = true;
}

void GuiSelectBox::update(GuiController *c) {
    if (bChanged) {
        showhide(this, showValues);
        if (showValues) {
            for (uint32_t i = 0; i < valueButtons.size(); ++i) { //TODO: only set when it really changed
                if (i == selected) {
                    valueButtons[i].valueButton->setImage(valueButtons[i].valueButtonCheckedImg);
                } else {
                    valueButtons[i].valueButton->setImage(valueButtons[i].valueButtonImg);
                }
            }
            valuesFrame.clearState(STATE_HIDDEN);
            DPADButtons.clearState(STATE_DISABLE_INPUT);
            valuesFrame.setEffect(EFFECT_FADE, 10, 255);
            valuesFrame.effectFinished.connect(this, &GuiSelectBox::OnValueCloseEffectFinish);
        } else {
            opened = false;
            valuesFrame.setState(STATE_HIDDEN);
            DPADButtons.setState(STATE_DISABLE_INPUT);
            valuesFrame.setEffect(EFFECT_FADE, -10, 0);
            valuesFrame.effectFinished.connect(this, &GuiSelectBox::OnValueOpenEffectFinish);
        }

        bChanged = false;
    }
    if (bSelectedChanged) {
        for (uint32_t i = 0; i < valueButtons.size(); ++i) {
            if (i == selected) {
                valueButtons[i].valueButton->setState(STATE_SELECTED);
            } else {
                valueButtons[i].valueButton->clearState(STATE_SELECTED);
            }
        }
    }
    topValueButton.setState(getState());
    GuiFrame::update(c);
}
