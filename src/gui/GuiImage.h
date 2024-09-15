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
    explicit GuiImage(const std::shared_ptr<GuiImageData> &img);

    //!\overload
    //!Creates an image filled with the specified color
    //!\param w Image width
    //!\param h Image height
    //!\param c Array with 4 x image color (BL, BR, TL, TR)
    GuiImage(uint32_t w, uint32_t h, const GX2Color &c, GuiImage::ImageTypes imgType = IMAGE_COLOR);

    GuiImage(uint32_t w, uint32_t h, const GX2Color *c, uint32_t colorCount = 1, GuiImage::ImageTypes imgType = IMAGE_COLOR);

    //!Destructor
    ~GuiImage() override;

    //!Constantly called to draw the image
    void draw(CVideo *pVideo) override;

    //!Gets the image data
    //!\return pointer to image data
    [[nodiscard]] GuiImageData *getImageData() const {
        return imageData.get();
    }

    //!Sets up a new image using the GuiImageData object specified
    //!\param img Pointer to GuiImageData object
    void setImageData(const std::shared_ptr<GuiImageData> &img);

    //!Change ImageColor
    void setImageColor(const GX2Color &c, int32_t idx = -1);

    virtual void setColorIntensity(const glm::vec4 &col) {
        colorIntensity = col;
    }

protected:
    void internalInit(uint32_t w, uint32_t h);

    ImageTypes imgType = IMAGE_TEXTURE;      //!< Type of image data (IMAGE_TEXTURE, IMAGE_COLOR, IMAGE_DATA)
    std::shared_ptr<GuiImageData> imageData; //!< Pointer to image data.

    //! Internally used variables for rendering
    uint8_t *colorVtxs = nullptr;

    uint32_t colorCount       = 0;
    bool colorVtxsDirty       = false;
    glm::vec3 positionOffsets = {};
    glm::vec3 scaleFactor     = {};
    glm::vec4 colorIntensity  = {};

    const float *texCoords = {};
    uint32_t vtxCount      = {};
    int32_t primitive      = {};
};

#endif
