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
#include <video/CVideo.h>
#include <video/shaders/ColorShader.h>
#include <video/shaders/Texture2DShader.h>

static const float fPiDiv180 = ((float) M_PI / 180.0f);

GuiImage::GuiImage(GuiImageData *img) {
    if (img && img->getTexture()) {
        width  = img->getWidth();
        height = img->getHeight();
    }

    internalInit(width, height);
    imageData = img;
}

GuiImage::GuiImage(int32_t w, int32_t h, const GX2Color &c, int32_t type) {
    internalInit(w, h);
    imgType    = type;
    colorCount = ColorShader::cuColorVtxsSize / ColorShader::cuColorAttrSize;

    colorVtxs = (uint8_t *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, colorCount * ColorShader::cuColorAttrSize);
    if (colorVtxs) {
        for (uint32_t i = 0; i < colorCount; i++) {
            setImageColor(c, i);
        }
    }
}

GuiImage::GuiImage(int32_t w, int32_t h, const GX2Color *c, uint32_t color_count, int32_t type) {
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
        colorVtxs = NULL;
    }
}

void GuiImage::internalInit(int32_t w, int32_t h) {
    imageData      = NULL;
    width          = w;
    height         = h;
    tileHorizontal = -1;
    tileVertical   = -1;
    imgType        = IMAGE_TEXTURE;
    colorVtxsDirty = false;
    colorVtxs      = NULL;
    colorCount     = 0;
    posVtxs        = NULL;
    texCoords      = NULL;
    vtxCount       = 4;
    primitive      = GX2_PRIMITIVE_MODE_QUADS;

    imageAngle      = 0.0f;
    blurDirection   = glm::vec3(0.0f);
    positionOffsets = glm::vec3(0.0f);
    scaleFactor     = glm::vec3(1.0f);
    colorIntensity  = glm::vec4(1.0f);
}

void GuiImage::setImageData(GuiImageData *img) {
    imageData = img;
    width     = 0;
    height    = 0;
    if (img && img->getTexture()) {
        width  = img->getWidth();
        height = img->getHeight();
    }
    imgType = IMAGE_TEXTURE;
}

GX2Color GuiImage::getPixel(int32_t x, int32_t y) {
    if (!imageData || this->getWidth() <= 0 || x < 0 || y < 0 || x >= this->getWidth() || y >= this->getHeight()) {
        return (GX2Color){
                0, 0, 0, 0};
    }

    uint32_t pitch     = imageData->getTexture()->surface.pitch;
    uint32_t *imagePtr = (uint32_t *) imageData->getTexture()->surface.image;

    uint32_t color_u32 = imagePtr[y * pitch + x];
    GX2Color color;
    color.r = (color_u32 >> 24) & 0xFF;
    color.g = (color_u32 >> 16) & 0xFF;
    color.b = (color_u32 >> 8) & 0xFF;
    color.a = (color_u32 >> 0) & 0xFF;
    return color;
}

void GuiImage::setPixel(int32_t x, int32_t y, const GX2Color &color) {
    if (!imageData || this->getWidth() <= 0 || x < 0 || y < 0 || x >= this->getWidth() || y >= this->getHeight()) {
        return;
    }


    uint32_t pitch          = imageData->getTexture()->surface.pitch;
    uint32_t *imagePtr      = (uint32_t *) imageData->getTexture()->surface.image;
    imagePtr[y * pitch + x] = (color.r << 24) | (color.g << 16) | (color.b << 8) | (color.a << 0);
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
    } else if (colorVtxs) {
        for (uint32_t i = 0; i < (ColorShader::cuColorVtxsSize / sizeof(uint8_t)); i += 4) {
            colorVtxs[i + 0] = c.r;
            colorVtxs[i + 1] = c.g;
            colorVtxs[i + 2] = c.b;
            colorVtxs[i + 3] = c.a;
        }
        colorVtxsDirty = true;
    }
}

void GuiImage::setSize(int32_t w, int32_t h) {
    width  = w;
    height = h;
}

void GuiImage::setPrimitiveVertex(int32_t prim, const float *posVtx, const float *texCoord, uint32_t vtxcount) {
    primitive = prim;
    vtxCount  = vtxcount;
    posVtxs   = posVtx;
    texCoords = texCoord;

    if (imgType == IMAGE_COLOR) {
        uint8_t *newColorVtxs = (uint8_t *) memalign(0x40, ColorShader::cuColorAttrSize * vtxCount);

        for (uint32_t i = 0; i < vtxCount; i++) {
            int32_t newColorIdx = (i << 2);
            int32_t colorIdx    = (i < colorCount) ? (newColorIdx) : ((colorCount - 1) << 2);

            newColorVtxs[newColorIdx + 0] = colorVtxs[colorIdx + 0];
            newColorVtxs[newColorIdx + 1] = colorVtxs[colorIdx + 1];
            newColorVtxs[newColorIdx + 2] = colorVtxs[colorIdx + 2];
            newColorVtxs[newColorIdx + 3] = colorVtxs[colorIdx + 3];
        }

        free(colorVtxs);
        colorVtxs      = newColorVtxs;
        colorCount     = vtxCount;
        colorVtxsDirty = true;
    }
}

void GuiImage::draw(CVideo *pVideo) {
    if (!this->isVisible() || tileVertical == 0 || tileHorizontal == 0) {
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
    imageAngle = DegToRad(getAngle());

    //	if(image && tileHorizontal > 0 && tileVertical > 0)
    //	{
    //		for(int32_t n=0; n<tileVertical; n++)
    //			for(int32_t i=0; i<tileHorizontal; i++)
    //			{
    //				if(bUnCut)
    //					Menu_DrawImg(image, width, height, format, currLeft+width*i, currTop+width*n, currZ, imageangle, currScaleX, currScaleY, currAlpha);
    //				else
    //					Menu_DrawImgCut(image, width, height, format, currLeft+width*i, currTop+width*n, currZ, imageangle, currScaleX, currScaleY, currAlpha, cutBoundsRect.x1(), cutBoundsRect.x2(), cutBoundsRect.y1(), cutBoundsRect.y2());
    //			}
    //	}
    //	else if(image && tileHorizontal > 0)
    //	{
    //		for(int32_t i=0; i<tileHorizontal; i++)
    //		{
    //			int32_t widthTile = (imageangle == 90 || imageangle == 270) ? height : width;
    //			if(bUnCut)
    //				Menu_DrawImg(image, width, height, format, currLeft+widthTile*i, currTop, currZ, imageangle, currScaleX, currScaleY, currAlpha);
    //			else
    //				Menu_DrawImgCut(image, width, height, format, currLeft+widthTile*i, currTop, currZ, imageangle, currScaleX, currScaleY, currAlpha, cutBoundsRect.x1(), cutBoundsRect.x2(), cutBoundsRect.y1(), cutBoundsRect.y2());
    //		}
    //	}
    //	else if(image && tileVertical > 0)
    //	{
    //		for(int32_t i=0; i<tileVertical; i++)
    //		{
    //			if(bUnCut)
    //				Menu_DrawImg(image, width, height, format, currLeft, currTop+height*i, currZ, imageangle, currScaleX, currScaleY, currAlpha);
    //			else
    //				Menu_DrawImgCut(image, width, height, format, currLeft, currTop+height*i, currZ, imageangle, currScaleX, currScaleY, currAlpha, cutBoundsRect.x1(), cutBoundsRect.x2(), cutBoundsRect.y1(), cutBoundsRect.y2());
    //		}
    //	}
    if (colorVtxsDirty && colorVtxs) {
        //! flush color vertex only on main GX2 thread
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, colorVtxs, colorCount * ColorShader::cuColorAttrSize);
        colorVtxsDirty = false;
    }

    if (imgType == IMAGE_COLOR && colorVtxs) {
        ColorShader::instance()->setShaders();
        ColorShader::instance()->setAttributeBuffer(colorVtxs, posVtxs, vtxCount);
        ColorShader::instance()->setAngle(imageAngle);
        ColorShader::instance()->setOffset(positionOffsets);
        ColorShader::instance()->setScale(scaleFactor);
        ColorShader::instance()->setColorIntensity(colorIntensity);
        ColorShader::instance()->draw(primitive, vtxCount);
    } else if (imageData) {
        Texture2DShader::instance()->setShaders();
        Texture2DShader::instance()->setAttributeBuffer(texCoords, posVtxs, vtxCount);
        Texture2DShader::instance()->setAngle(imageAngle);
        Texture2DShader::instance()->setOffset(positionOffsets);
        Texture2DShader::instance()->setScale(scaleFactor);
        Texture2DShader::instance()->setColorIntensity(colorIntensity);
        Texture2DShader::instance()->setBlurring(blurDirection);
        Texture2DShader::instance()->setTextureAndSampler(imageData->getTexture(), imageData->getSampler());
        Texture2DShader::instance()->draw(primitive, vtxCount);
    }
}
