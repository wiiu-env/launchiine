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
#ifndef GUI_IMAGEDATA_H_
#define GUI_IMAGEDATA_H_

#include <gd.h>
#include <gui/GuiElement.h>
#include <gui/gx2_ext.h>
#include <gx2/texture.h>
#include <memory>
#include <span>

class GuiImageData : public GuiElement {
public:
    //!Constructor
    GuiImageData();

    //!\param img Image data
    //!\param imgSize The image size
     explicit GuiImageData(std::span<const uint8_t> img, GX2TexClampMode textureClamp = GX2_TEX_CLAMP_MODE_CLAMP, GX2SurfaceFormat textureFormat = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);

    //!Destructor
    ~GuiImageData() override;

    //!Load image from buffer
    //!\param img Image data
    //!\param imgSize The image size
    bool loadImage(std::span<const uint8_t> img, GX2TexClampMode textureClamp = GX2_TEX_CLAMP_MODE_CLAMP, GX2SurfaceFormat textureFormat = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8);

    //! getter functions
    [[nodiscard]] virtual const GX2Texture *getTexture() const {
        if (!textureWrapper) {
            return nullptr;
        }
        return textureWrapper->getTexture();
    };

    [[nodiscard]] virtual const GX2Sampler *getSampler() const {
        return sampler.get();
    };

    //!Gets the image width
    //!\return image width
    float getWidth() override {
        if (textureWrapper) {
            return textureWrapper->getTexture()->surface.width;
        } else {
            return 0;
        }
    };

    //!Gets the image height
    //!\return image height
    float getHeight() override {
        if (textureWrapper) {
            return textureWrapper->getTexture()->surface.height;
        } else {
            return 0;
        }
    };

    //! release memory of the image data
    void releaseData();

private:
    class GX2TextureImageDataWrapper {
    public:
        [[nodiscard]] GX2Texture *getTexture() {
            return &texture;
        }

        bool InitTexture(uint32_t width, uint32_t height, GX2SurfaceFormat textureFormat) {
            GX2InitTexture(&texture, width, height, 1, 0, textureFormat, GX2_SURFACE_DIM_TEXTURE_2D, GX2_TILE_MODE_LINEAR_ALIGNED);

            //! allocate memory for the surface
            memoryType            = MEM2;
            texture.surface.image = MEM2_alloc(texture.surface.alignment, texture.surface.imageSize);
            //! try MEM1 on failure
            if (!texture.surface.image) {
                memoryType            = MEM1;
                texture.surface.image = MEM1_alloc(texture.surface.imageSize, texture.surface.alignment);
            }
            //! try MEM bucket on failure
            if (!texture.surface.image) {
                memoryType            = MEM_BUCKET;
                texture.surface.image = MEMBucket_alloc(texture.surface.imageSize, texture.surface.alignment);
            }
            //! check if memory is available for image
            if (!texture.surface.image) {
                return false;
            }
            return true;
        }

        ~GX2TextureImageDataWrapper() {
            if (texture.surface.image) {
                switch (memoryType) {
                    default:
                    case MEM2:
                        MEM2_free(texture.surface.image);
                        break;
                    case MEM1:
                        MEM1_free(texture.surface.image);
                        break;
                    case MEM_BUCKET:
                        MEMBucket_free(texture.surface.image);
                        break;
                }
            }
        }

        enum MemoryType {
            MEM1,
            MEM2,
            MEM_BUCKET
        };

    private:
        alignas(0x40) GX2Texture texture;
        MemoryType memoryType;
    };

    void gdImageToUnormR8G8B8A8(gdImagePtr gdImg, uint32_t *imgBuffer, uint32_t width, uint32_t height, uint32_t pitch);

    void gdImageToUnormR5G6B5(gdImagePtr gdImg, uint16_t *imgBuffer, uint32_t width, uint32_t height, uint32_t pitch);

    std::unique_ptr<GX2TextureImageDataWrapper> textureWrapper;
    std::unique_ptr<GX2Sampler> sampler;
};

#endif
