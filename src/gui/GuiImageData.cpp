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
#include "system/memory.h"
#include <gui/GuiImageData.h>

/**
 * Constructor for the GuiImageData class.
 */
GuiImageData::GuiImageData() = default;

/**
 * Constructor for the GuiImageData class.
 */
GuiImageData::GuiImageData(std::span<const uint8_t> data, GX2TexClampMode textureClamp, GX2SurfaceFormat textureFormat) {
    if (!loadImage(data, textureClamp, textureFormat)) {
        releaseData();
    }
}

/**
 * Destructor for the GuiImageData class.
 */
GuiImageData::~GuiImageData() {
    releaseData();
}

void GuiImageData::releaseData() {
    textureWrapper.reset();
    sampler.reset();
}

bool GuiImageData::loadImage(std::span<const uint8_t> img, GX2TexClampMode textureClamp, GX2SurfaceFormat textureFormat) {
    if (img.empty() || img.size() < 8) {
        return false;
    }

    releaseData();
    gdImagePtr gdImg = nullptr;

    if (img[0] == 0xFF && img[1] == 0xD8) {
        //! not needed for now therefore comment out to safe ELF size
        //! if needed uncomment, adds 200 kb to the ELF size
        // IMAGE_JPEG
        gdImg = gdImageCreateFromJpegPtr(img.size(), (void *) img.data());
    } else if (img[0] == 'B' && img[1] == 'M') {
        // IMAGE_BMP
        gdImg = gdImageCreateFromBmpPtr(img.size(), (void *) img.data());
    } else if (img[0] == 0x89 && img[1] == 'P' && img[2] == 'N' && img[3] == 'G') {
        // IMAGE_PNG
        gdImg = gdImageCreateFromPngPtr(img.size(), (void *) img.data());
    }
    //!This must be last since it can also intefere with outher formats
    else if (img[0] == 0x00) {
        // Try loading TGA image
        gdImg = gdImageCreateFromTgaPtr(img.size(), (void *) img.data());
    }

    if (gdImg == nullptr) {
        return false;
    }

    uint32_t width  = (gdImageSX(gdImg));
    uint32_t height = (gdImageSY(gdImg));

    //! Initialize texture
    textureWrapper = std::make_unique<GX2TextureImageDataWrapper>();
    if (!textureWrapper->InitTexture(width, height, textureFormat)) {
        gdImageDestroy(gdImg);
        return false;
    }

    GX2Texture *texture = textureWrapper->getTexture();

    //! if this fails something went horribly wrong
    if (texture->surface.imageSize == 0) {
        gdImageDestroy(gdImg);
        return false;
    }

    //! set mip map data pointer
    texture->surface.mipmaps = nullptr;
    //! convert image to texture
    switch (textureFormat) {
        default:
        case GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8:
            gdImageToUnormR8G8B8A8(gdImg, (uint32_t *) texture->surface.image, texture->surface.width, texture->surface.height, texture->surface.pitch);
            break;
        case GX2_SURFACE_FORMAT_UNORM_R5_G6_B5:
            gdImageToUnormR5G6B5(gdImg, (uint16_t *) texture->surface.image, texture->surface.width, texture->surface.height, texture->surface.pitch);
            break;
    }

    //! free memory of image as its not needed anymore
    gdImageDestroy(gdImg);

    //! invalidate the memory
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, texture->surface.image, texture->surface.imageSize);
    //! initialize the sampler
    sampler = std::make_unique<GX2Sampler>();
    GX2InitSampler(sampler.get(), textureClamp, GX2_TEX_XY_FILTER_MODE_LINEAR);
    return true;
}

void GuiImageData::gdImageToUnormR8G8B8A8(gdImagePtr gdImg, uint32_t *imgBuffer, uint32_t width, uint32_t height, uint32_t pitch) {
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            uint32_t pixel = gdImageGetPixel(gdImg, x, y);

            uint8_t a = 254 - 2 * ((uint8_t) gdImageAlpha(gdImg, pixel));
            if (a == 254) { a++; }

            uint8_t r = gdImageRed(gdImg, pixel);
            uint8_t g = gdImageGreen(gdImg, pixel);
            uint8_t b = gdImageBlue(gdImg, pixel);

            imgBuffer[y * pitch + x] = (r << 24) | (g << 16) | (b << 8) | (a);
        }
    }
}

//! TODO: figure out why this seems to not work correct yet
void GuiImageData::gdImageToUnormR5G6B5(gdImagePtr gdImg, uint16_t *imgBuffer, uint32_t width, uint32_t height, uint32_t pitch) {
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            uint32_t pixel = gdImageGetPixel(gdImg, x, y);

            uint8_t r = gdImageRed(gdImg, pixel);
            uint8_t g = gdImageGreen(gdImg, pixel);
            uint8_t b = gdImageBlue(gdImg, pixel);

            imgBuffer[y * pitch + x] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
        }
    }
}
