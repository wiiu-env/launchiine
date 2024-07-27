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
#ifndef GUI_SELECTBOX_H_
#define GUI_SELECTBOX_H_

#include <gui/Gui.h>
#include <gui/GuiImage.h>
#include <gui/GuiImageData.h>

//!A simple CheckBox
class GuiSelectBox : public GuiFrame, public sigslot::has_slots<> {
public:
    //!Constructor
    //!\param checked Checked
    GuiSelectBox(GuiImage *background, std::string caption, float width = 0.0f, float height = 0.0f, GuiFrame *parent = 0);

    //!Destructor
    virtual ~GuiSelectBox();

    sigslot::signal2<GuiSelectBox *, std::string> valueChanged;
    sigslot::signal2<GuiSelectBox *, bool> showhide;

    void setImageTopBackground(GuiImage *img) {
        topBackgroundImg = img;
        topValueButton.setImage(img);
    }

    void setImageTopHighlighted(GuiImage *img) {
        topHighlightedImg = img;
        topValueButton.setIconOver(img);
    }

    void setImageValueBackground(GuiImageData *img) {
        valueImageData = img;
    }

    void setImageValueHighlighted(GuiImageData *img) {
        valueHighlightedImageData = img;
    }

    void setImageValueSelected(GuiImageData *img) {
        valueSelectedImageData = img;
    }

    void setSoundClick(GuiSound *snd) {
        buttonClickSound = snd;
        topValueButton.setSoundClick(snd);
    }

    void OnTopValueClicked(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void Init(std::map<std::string, std::string> values, int32_t valueID);

    void setState(int32_t s, int32_t c = -1);

    virtual void setSize(float width, float height);

    virtual float getTopValueHeight();

    virtual float getTopValueWidth();

    virtual float getHeight();

    virtual float getWidth();

protected:
    void DeleteValueData();

    void update(GuiController *c);

    void OnValueClicked(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnDPADClick(GuiButton *button, const GuiController *controller, GuiTrigger *trigger);

    void OnValueOpenEffectFinish(GuiElement *element);

    void OnValueCloseEffectFinish(GuiElement *element);

    void ShowHideValues(bool showhide);

    void SelectValue(uint32_t value);

    uint32_t selected;
    bool bChanged;
    bool bSelectedChanged;
    bool showValues;
    bool opened;
    std::string captionText;
    GuiFrame valuesFrame;
    GuiImage *topBackgroundImg  = NULL;
    GuiImage *topHighlightedImg = NULL;

    GuiButton topValueButton;
    GuiImageData *valueImageData            = NULL;
    GuiImageData *valueSelectedImageData    = NULL;
    GuiImageData *valueHighlightedImageData = NULL;
    GuiText topValueText;

    GuiTrigger touchTrigger;
    GuiTrigger wpadTouchTrigger;

    GuiTrigger buttonATrigger;
    GuiTrigger buttonBTrigger;
    GuiTrigger buttonLeftTrigger;
    GuiTrigger buttonRightTrigger;
    GuiTrigger buttonUpTrigger;
    GuiTrigger buttonDownTrigger;

    GuiButton DPADButtons;

    GuiSound *buttonClickSound;

    typedef struct {
        GuiImage *valueButtonImg;
        GuiImage *valueButtonCheckedImg;
        GuiImage *valueButtonHighlightedImg;
        GuiButton *valueButton;
        GuiText *valueButtonText;
    } SelectBoxValueButton;

    std::map<GuiButton *, std::string> buttonToValue;
    std::vector<SelectBoxValueButton> valueButtons;
};

#endif
