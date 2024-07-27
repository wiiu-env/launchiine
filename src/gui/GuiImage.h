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
#ifndef GUI_IMAGE_H_
#define GUI_IMAGE_H_

#include <gui/GuiElement.h>
#include <gui/GuiImageData.h>
#include <gui/gx2_ext.h>
#include <video/shaders/Shader.h>

//!Display, manage, and manipulate images in the GUI
class GuiImage : public GuiElement {
public:
    enum ImageTypes {
        IMAGE_TEXTURE,
        IMAGE_COLOR
    };

    //!\overload
    //!\param img Pointer to GuiImageData element
    explicit GuiImage(GuiImageData *img);

    //!\overload
    //!Creates an image filled with the specified color
    //!\param w Image width
    //!\param h Image height
    //!\param c Array with 4 x image color (BL, BR, TL, TR)
    GuiImage(int32_t w, int32_t h, const GX2Color &c, int32_t imgType = IMAGE_COLOR);

    GuiImage(int32_t w, int32_t h, const GX2Color *c, uint32_t colorCount = 1, int32_t imgType = IMAGE_COLOR);

    //!Destructor
    ~GuiImage() override;

    //!Sets the number of times to draw the image horizontally
    //!\param t Number of times to draw the image
    void setTileHorizontal(int32_t t) {
        tileHorizontal = t;
    }

    //!Sets the number of times to draw the image vertically
    //!\param t Number of times to draw the image
    void setTileVertical(int32_t t) {
        tileVertical = t;
    }

    //!Constantly called to draw the image
    void draw(CVideo *pVideo) override;

    //!Gets the image data
    //!\return pointer to image data
    [[nodiscard]] GuiImageData *getImageData() const {
        return imageData;
    }

    //!Sets up a new image using the GuiImageData object specified
    //!\param img Pointer to GuiImageData object
    void setImageData(GuiImageData *img);

    //!Gets the pixel color at the specified coordinates of the image
    //!\param x X coordinate
    //!\param y Y coordinate
    GX2Color getPixel(int32_t x, int32_t y);

    //!Sets the pixel color at the specified coordinates of the image
    //!\param x X coordinate
    //!\param y Y coordinate
    //!\param color Pixel color
    void setPixel(int32_t x, int32_t y, const GX2Color &color);

    //!Change ImageColor
    void setImageColor(const GX2Color &c, int32_t idx = -1);

    //!Change ImageColor
    void setSize(int32_t w, int32_t h);

    void setPrimitiveVertex(int32_t prim, const float *pos, const float *tex, uint32_t count);

    void setBlurDirection(uint8_t dir, float value) {
        if (dir < 2) {
            blurDirection[dir] = value;
        }
    }

    void setColorIntensity(const glm::vec4 &col) {
        colorIntensity = col;
    }

protected:
    void internalInit(int32_t w, int32_t h);

    int32_t imgType;         //!< Type of image data (IMAGE_TEXTURE, IMAGE_COLOR, IMAGE_DATA)
    GuiImageData *imageData; //!< Poiner to image data. May be shared with GuiImageData data
    int32_t tileHorizontal;  //!< Number of times to draw (tile) the image horizontally
    int32_t tileVertical;    //!< Number of times to draw (tile) the image vertically

    //! Internally used variables for rendering
    uint8_t *colorVtxs;
    uint32_t colorCount;
    bool colorVtxsDirty;
    glm::vec3 positionOffsets;
    glm::vec3 scaleFactor;
    glm::vec4 colorIntensity;
    float imageAngle;
    glm::vec3 blurDirection;

    const float *posVtxs;
    const float *texCoords;
    uint32_t vtxCount;
    int32_t primitive;
};

#endif
