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
#ifndef WPAD_CONTROLLER_H_
#define WPAD_CONTROLLER_H_

#include <gui/GuiController.h>
#include <padscore/kpad.h>
#include <padscore/wpad.h>

class WPadController : public GuiController {
public:
    //!Constructor
    WPadController(int32_t channel)
        : GuiController(channel) {
        memset(&kpadData, 0, sizeof(kpadData));
    }

    //!Destructor
    virtual ~WPadController() {}

    uint32_t remapWiiMoteButtons(uint32_t buttons) {
        uint32_t conv_buttons = 0;

        if (buttons & WPAD_BUTTON_LEFT) {
            conv_buttons |= GuiTrigger::BUTTON_LEFT;
        }

        if (buttons & WPAD_BUTTON_RIGHT) {
            conv_buttons |= GuiTrigger::BUTTON_RIGHT;
        }

        if (buttons & WPAD_BUTTON_DOWN) {
            conv_buttons |= GuiTrigger::BUTTON_DOWN;
        }

        if (buttons & WPAD_BUTTON_UP) {
            conv_buttons |= GuiTrigger::BUTTON_UP;
        }

        if (buttons & WPAD_BUTTON_PLUS) {
            conv_buttons |= GuiTrigger::BUTTON_PLUS;
        }

        if (buttons & WPAD_BUTTON_2) {
            conv_buttons |= GuiTrigger::BUTTON_2;
        }

        if (buttons & WPAD_BUTTON_1) {
            conv_buttons |= GuiTrigger::BUTTON_1;
        }

        if (buttons & WPAD_BUTTON_B) {
            conv_buttons |= GuiTrigger::BUTTON_B;
        }

        if (buttons & WPAD_BUTTON_A) {
            conv_buttons |= GuiTrigger::BUTTON_A;
        }

        if (buttons & WPAD_BUTTON_MINUS) {
            conv_buttons |= GuiTrigger::BUTTON_MINUS;
        }

        if (buttons & WPAD_BUTTON_Z) {
            conv_buttons |= GuiTrigger::BUTTON_Z;
        }

        if (buttons & WPAD_BUTTON_C) {
            conv_buttons |= GuiTrigger::BUTTON_C;
        }

        if (buttons & WPAD_BUTTON_HOME) {
            conv_buttons |= GuiTrigger::BUTTON_HOME;
        }

        return conv_buttons;
    }

    uint32_t remapClassicButtons(uint32_t buttons) {
        uint32_t conv_buttons = 0;

        if (buttons & WPAD_CLASSIC_BUTTON_LEFT) {
            conv_buttons |= GuiTrigger::BUTTON_LEFT;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_RIGHT) {
            conv_buttons |= GuiTrigger::BUTTON_RIGHT;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_DOWN) {
            conv_buttons |= GuiTrigger::BUTTON_DOWN;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_UP) {
            conv_buttons |= GuiTrigger::BUTTON_UP;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_PLUS) {
            conv_buttons |= GuiTrigger::BUTTON_PLUS;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_X) {
            conv_buttons |= GuiTrigger::BUTTON_X;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_Y) {
            conv_buttons |= GuiTrigger::BUTTON_Y;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_B) {
            conv_buttons |= GuiTrigger::BUTTON_B;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_A) {
            conv_buttons |= GuiTrigger::BUTTON_A;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_MINUS) {
            conv_buttons |= GuiTrigger::BUTTON_MINUS;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_HOME) {
            conv_buttons |= GuiTrigger::BUTTON_HOME;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_ZR) {
            conv_buttons |= GuiTrigger::BUTTON_ZR;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_ZL) {
            conv_buttons |= GuiTrigger::BUTTON_ZL;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_R) {
            conv_buttons |= GuiTrigger::BUTTON_R;
        }

        if (buttons & WPAD_CLASSIC_BUTTON_L) {
            conv_buttons |= GuiTrigger::BUTTON_L;
        }

        return conv_buttons;
    }

    WPADChan getChanByInt(int32_t chan) {
        if (chan == 0) {
            return WPAD_CHAN_0;
        }
        if (chan == 1) {
            return WPAD_CHAN_1;
        }
        if (chan == 2) {
            return WPAD_CHAN_3;
        }
        if (chan == 3) {
            return WPAD_CHAN_3;
        }
        return WPAD_CHAN_0;
    }

    bool update(int32_t width, int32_t height) {
        lastData = data;
        WPADExtensionType controller_type;

        //! check if the controller is connected
        if (WPADProbe(getChanByInt(chanIdx - 1), &controller_type) != 0) {
            return false;
        }

        KPADRead(getChanByInt(chanIdx - 1), &kpadData, 1);

        if (kpadData.extensionType <= 1) {
            data.buttons_r = remapWiiMoteButtons(kpadData.release);
            data.buttons_h = remapWiiMoteButtons(kpadData.hold);
            data.buttons_d = remapWiiMoteButtons(kpadData.trigger);
        } else {
            data.buttons_r = remapClassicButtons(kpadData.classic.release);
            data.buttons_h = remapClassicButtons(kpadData.classic.hold);
            data.buttons_d = remapClassicButtons(kpadData.classic.trigger);
        }

        data.validPointer = (kpadData.posValid == 1 || kpadData.posValid == 2) && (kpadData.pos.x >= -1.0f && kpadData.pos.x <= 1.0f) && (kpadData.pos.y >= -1.0f && kpadData.pos.y <= 1.0f);
        //! calculate the screen offsets if pointer is valid else leave old value
        if (data.validPointer) {
            data.x = (width >> 1) * kpadData.pos.x;
            data.y = (height >> 1) * (-kpadData.pos.y);

            if (kpadData.angle.y > 0.0f) {
                data.pointerAngle = (-kpadData.angle.x + 1.0f) * 0.5f * 180.0f;
            } else {
                data.pointerAngle = (kpadData.angle.x + 1.0f) * 0.5f * 180.0f - 180.0f;
            }
        }

        return true;
    }

private:
    KPADStatus kpadData;
    uint32_t lastButtons;
};

#endif
