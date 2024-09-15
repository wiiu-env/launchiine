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
#include "utils/utils.h"
#include <gui/GuiImage.h>
#include <stdexcept>
#include <video/CVideo.h>
#include <video/shaders/ColorShader.h>
#include <video/shaders/Texture2DShader.h>

GuiImage::GuiImage(const std::shared_ptr<GuiImageData> &img) {
    if (img && img->getTexture()) {
        width  = img->getWidth();
        height = img->getHeight();
    }

    internalInit((uint32_t) width, (uint32_t) height);
    imageData = img;
}

GuiImage::GuiImage(uint32_t w, uint32_t h, const GX2Color &c, GuiImage::ImageTypes type) {
    internalInit(w, h);
    imgType    = type;
    colorCount = ColorShader::cuColorVtxsSize / ColorShader::cuColorAttrSize;

    colorVtxs = (uint8_t *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, colorCount * ColorShader::cuColorAttrSize);
    if (!colorVtxs) {
        throw std::runtime_error("Failed to alloc colorVtxs for GuiImage");
    }
    for (int32_t i = 0; i < colorCount; i++) {
        setImageColor(c, i);
    }
}

GuiImage::GuiImage(uint32_t w, uint32_t h, const GX2Color *c, uint32_t color_count, GuiImage::ImageTypes type) {
    internalInit(w, h);
    imgType    = type;
    colorCount = ColorShader::cuColorVtxsSize / ColorShader::cuColorAttrSize;
    if (colorCount < color_count) {
        colorCount = color_count;
    }

    colorVtxs = (uint8_t *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, colorCount * ColorShader::cuColorAttrSize);
    if (colorVtxs) {
        for (uint32_t i = 0; i < colorCount; i++) {
            // take the last as reference if not enough colors defined
            int32_t idx = (i < color_count) ? i : (color_count - 1);
            setImageColor(c[idx], i);
        }
    }
}

/**
 * Destructor for the GuiImage class.
 */
GuiImage::~GuiImage() {
    if (colorVtxs) {
        free(colorVtxs);
        colorVtxs = nullptr;
    }
}

void GuiImage::internalInit(uint32_t w, uint32_t h) {
    imageData      = nullptr;
    width          = (float) w;
    height         = (float) h;
    imgType        = IMAGE_TEXTURE;
    colorVtxsDirty = false;
    colorVtxs      = nullptr;
    colorCount     = 0;
    texCoords      = nullptr;
    vtxCount       = 4;
    primitive      = GX2_PRIMITIVE_MODE_QUADS;

    positionOffsets = glm::vec3(0.0f);
    scaleFactor     = glm::vec3(1.0f);
    colorIntensity  = glm::vec4(1.0f);
}

void GuiImage::setImageData(const std::shared_ptr<GuiImageData> &img) {
    imageData = img;
    width     = 0;
    height    = 0;
    if (img->getTexture()) {
        width  = img->getWidth();
        height = img->getHeight();
    }
    imgType = IMAGE_TEXTURE;
}

void GuiImage::setImageColor(const GX2Color &c, int32_t idx) {
    if (!colorVtxs) {
        return;
    }

    if (idx >= 0 && idx < (int32_t) colorCount) {
        colorVtxs[(idx << 2) + 0] = c.r;
        colorVtxs[(idx << 2) + 1] = c.g;
        colorVtxs[(idx << 2) + 2] = c.b;
        colorVtxs[(idx << 2) + 3] = c.a;
        colorVtxsDirty            = true;
    } else {
        for (uint32_t i = 0; i < (ColorShader::cuColorVtxsSize / sizeof(uint8_t)); i += 4) {
            colorVtxs[i + 0] = c.r;
            colorVtxs[i + 1] = c.g;
            colorVtxs[i + 2] = c.b;
            colorVtxs[i + 3] = c.a;
        }
        colorVtxsDirty = true;
    }
}


void GuiImage::draw(CVideo *pVideo) {
    if (!this->isVisible()) {
        return;
    }

    float currScaleX = getScaleX();
    float currScaleY = getScaleY();

    positionOffsets[0] = getCenterX() * pVideo->getWidthScaleFactor() * 2.0f;
    positionOffsets[1] = getCenterY() * pVideo->getHeightScaleFactor() * 2.0f;
    positionOffsets[2] = getDepth() * pVideo->getDepthScaleFactor() * 2.0f;

    scaleFactor[0] = currScaleX * getWidth() * pVideo->getWidthScaleFactor();
    scaleFactor[1] = currScaleY * getHeight() * pVideo->getHeightScaleFactor();
    scaleFactor[2] = getScaleZ();

    //! add other colors intensities parameters
    colorIntensity[3] = getAlpha();

    //! angle of the object
    float imageAngle = DegToRad(getAngle());

    if (colorVtxsDirty && colorVtxs) {
        //! flush color vertex only on main GX2 thread
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, colorVtxs, colorCount * ColorShader::cuColorAttrSize);
        colorVtxsDirty = false;
    }

    if (imgType == IMAGE_COLOR && colorVtxs) {
        ColorShader::instance()->setShaders();
        ColorShader::instance()->setAttributeBuffer(colorVtxs);
        ColorShader::instance()->setAngle(imageAngle);
        ColorShader::instance()->setOffset(positionOffsets);
        ColorShader::instance()->setScale(scaleFactor);
        ColorShader::instance()->setColorIntensity(colorIntensity);
        ColorShader::instance()->draw(primitive);
    } else if (imageData) {
        Texture2DShader::instance()->setShaders();
        Texture2DShader::instance()->setAttributeBuffer(texCoords);
        Texture2DShader::instance()->setAngle(imageAngle);
        Texture2DShader::instance()->setOffset(positionOffsets);
        Texture2DShader::instance()->setScale(scaleFactor);
        Texture2DShader::instance()->setColorIntensity(colorIntensity);
        Texture2DShader::instance()->setTextureAndSampler(imageData->getTexture(), imageData->getSampler());
        Texture2DShader::instance()->draw(primitive);
    }
}
