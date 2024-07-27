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
#ifndef GUI_TEXT_H_
#define GUI_TEXT_H_

#include <gui/GuiElement.h>
#include <mutex>

//!Forward declaration
class FreeTypeGX;

//!Display, manage, and manipulate text in the GUI
class GuiText : public GuiElement {
public:
    //!Constructor
    GuiText();

    //!\param t Text
    //!\param s Font size
    //!\param c Font color
    GuiText(const char *t, int32_t s, const glm::vec4 &c);

    //!\overload
    //!\param t Text
    //!\param s Font size
    //!\param c Font color
    GuiText(const wchar_t *t, int32_t s, const glm::vec4 &c);

    //!\overload
    //!\Assumes SetPresets() has been called to setup preferred text attributes
    //!\param t Text
    GuiText(const char *t);

    //!Destructor
    virtual ~GuiText();

    //!Sets the text of the GuiText element
    //!\param t Text
    virtual void setText(const char *t);

    virtual void setText(const wchar_t *t);

    virtual void setTextf(const char *format, ...) __attribute__((format(printf, 2, 3)));

    //!Sets up preset values to be used by GuiText(t)
    //!Useful when printing multiple text elements, all with the same attributes set
    //!\param sz Font size
    //!\param c Font color
    //!\param w Maximum width of texture image (for text wrapping)
    //!\param wrap Wrapmode when w>0
    //!\param a Text alignment
    static void setPresets(int32_t sz, const glm::vec4 &c, int32_t w, int32_t a);

    static void setPresetFont(FreeTypeGX *font);

    //!Sets the font size
    //!\param s Font size
    void setFontSize(int32_t s);

    //!Sets the maximum width of the drawn texture image
    //!If the text exceeds this, it is wrapped to the next line
    //!\param w Maximum width
    //!\param m WrapMode
    void setMaxWidth(int32_t w = 0, int32_t m = WRAP);

    //!Sets the font color
    //!\param c Font color
    void setColor(const glm::vec4 &c);

    void setBlurGlowColor(float blurIntensity, const glm::vec4 &c);

    void setTextBlur(float blur) {
        defaultBlur = blur;
    }

    //!Get the original text as char
    virtual const wchar_t *getText() const {
        return text;
    }

    virtual std::string toUTF8(void) const;

    //!Get the Horizontal Size of Text
    int32_t getTextWidth();

    int32_t getTextWidth(int32_t ind);

    //!Get the max textwidth
    int32_t getTextMaxWidth() {
        return maxWidth;
    }

    float getLineHeight() {
        return (float) currentSize;
    }

    float getTextHeight();

    void setSSAA(int32_t ssaa) {
        this->internalSSAA = ssaa;
    };

    //!Get fontsize
    int32_t getFontSize() {
        return currentSize;
    };

    //!Set max lines to draw
    void setLinesToDraw(int32_t l) {
        linestodraw = l;
    }

    //!Get current Textline (for position calculation)
    const wchar_t *getDynText(int32_t ind = 0);

    virtual const wchar_t *getTextLine(int32_t ind) {
        return getDynText(ind);
    };

    //!Change the font
    bool setFont(FreeTypeGX *font);

    //! virtual function used in child classes
    virtual int32_t getStartWidth() {
        return 0;
    };

    //!Constantly called to draw the text
    void draw(CVideo *pVideo) override;

    virtual float getWidth() {
        return getTextWidth();
    }

    virtual float getHeight() {
        return getTextHeight();
    }

    virtual void setSize(float w, float h) {
        //! We calculate the size based on the text.
        this->width  = 0;
        this->height = 0;
    }

    virtual float getCenterY(void);

    virtual float getCenterX(void);

    //! text enums
    enum {
        WRAP,
        DOTTED,
        SCROLL_HORIZONTAL,
        SCROLL_NONE
    };

protected:
    static FreeTypeGX *presentFont;
    static int32_t presetSSAA;
    static int32_t presetSize;
    static int32_t presetMaxWidth;
    static float presetInternalRenderingScale;
    static int32_t presetAlignment;
    static GX2ColorF32 presetColor;

    //!Clear the dynamic text
    void clearDynamicText();

    //!Create a dynamic dotted text if the text is too long
    void makeDottedText();

    //!Scroll the text once
    void scrollText(uint32_t frameCount);

    //!Wrap the text to several lines
    void wrapText();

    mutable std::recursive_mutex textMutex;

    wchar_t *text;
    std::vector<wchar_t *> textDyn;
    std::vector<uint16_t> textDynWidth;
    int32_t wrapMode;               //!< Wrapping toggle
    int32_t textScrollPos;          //!< Current starting index of text string for scrolling
    int32_t textScrollInitialDelay; //!< Delay to wait before starting to scroll
    int32_t textScrollDelay;        //!< Scrolling speed
    int32_t maxWidth;               //!< Maximum width of the generated text object (for text wrapping)
    FreeTypeGX *font;
    int32_t currentSize;
    int32_t linestodraw;
    glm::vec4 color;
    float defaultBlur;
    float blurGlowIntensity;
    float blurAlpha;
    glm::vec4 blurGlowColor;
    int32_t internalSSAA;
};

#endif
