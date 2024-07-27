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
#include <cstdint>
#include "system/memory.h"
#include <video/CVideo.h>
#include <video/shaders/ColorShader.h>
#include <video/shaders/FXAAShader.h>
#include <video/shaders/Shader3D.h>
#include <video/shaders/ShaderFractalColor.h>
#include <video/shaders/Texture2DShader.h>
#include <malloc.h>
#include <string.h>

CVideo::CVideo(int32_t forceTvScanMode, int32_t forceDrcScanMode) {
    tvEnabled  = false;
    drcEnabled = false;

    //! allocate MEM2 command buffer memory
    gx2CommandBuffer = MEM2_alloc(GX2_COMMAND_BUFFER_SIZE, 0x40);

    //! initialize GX2 command buffer
    uint32_t gx2_init_attributes[9];
    gx2_init_attributes[0] = GX2_INIT_CMD_BUF_BASE;
    gx2_init_attributes[1] = (uint32_t) gx2CommandBuffer;
    gx2_init_attributes[2] = GX2_INIT_CMD_BUF_POOL_SIZE;
    gx2_init_attributes[3] = GX2_COMMAND_BUFFER_SIZE;
    gx2_init_attributes[4] = GX2_INIT_ARGC;
    gx2_init_attributes[5] = 0;
    gx2_init_attributes[6] = GX2_INIT_ARGV;
    gx2_init_attributes[7] = 0;
    gx2_init_attributes[8] = GX2_INIT_END;
    GX2Init(gx2_init_attributes);

    uint32_t scanBufferSize = 0;
    uint32_t scaleNeeded    = 0;

    int32_t tvScanMode  = ((forceTvScanMode >= 0) ? forceTvScanMode : (int32_t) GX2GetSystemTVScanMode());
    int32_t drcScanMode = ((forceDrcScanMode >= 0) ? forceDrcScanMode : (int32_t) GX2GetSystemDRCScanMode());

    int32_t tvRenderMode;
    uint32_t tvWidth  = 0;
    uint32_t tvHeight = 0;

    switch (tvScanMode) {
        case GX2_TV_SCAN_MODE_480I:
        case GX2_TV_SCAN_MODE_480P:
            tvWidth      = 854;
            tvHeight     = 480;
            tvRenderMode = GX2_TV_RENDER_MODE_WIDE_480P;
            break;
        case GX2_TV_SCAN_MODE_1080I:
        case GX2_TV_SCAN_MODE_1080P:
            tvWidth      = 1920;
            tvHeight     = 1080;
            tvRenderMode = GX2_TV_RENDER_MODE_WIDE_1080P;
            break;
        case GX2_TV_SCAN_MODE_720P:
        default:
            tvWidth      = 1280;
            tvHeight     = 720;
            tvRenderMode = GX2_TV_RENDER_MODE_WIDE_720P;
            break;
    }

    int32_t tvAAMode  = GX2_AA_MODE1X;
    int32_t drcAAMode = GX2_AA_MODE4X;

    //! calculate the scale factor for later texture resize
    widthScaleFactor  = 1.0f / (float) tvWidth;
    heightScaleFactor = 1.0f / (float) tvHeight;
    depthScaleFactor  = widthScaleFactor;

    //! calculate the size needed for the TV scan buffer and allocate the buffer from bucket memory
    GX2CalcTVSize((GX2TVRenderMode) tvRenderMode, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE, &scanBufferSize, &scaleNeeded);
    tvScanBuffer = MEMBucket_alloc(scanBufferSize, GX2_SCAN_BUFFER_ALIGNMENT);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, tvScanBuffer, scanBufferSize);
    GX2SetTVBuffer(tvScanBuffer, scanBufferSize, (GX2TVRenderMode) tvRenderMode, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE);

    //! calculate the size needed for the DRC scan buffer and allocate the buffer from bucket memory
    GX2CalcDRCSize((GX2DrcRenderMode) drcScanMode, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE, &scanBufferSize, &scaleNeeded);
    drcScanBuffer = MEMBucket_alloc(scanBufferSize, GX2_SCAN_BUFFER_ALIGNMENT);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, drcScanBuffer, scanBufferSize);
    GX2SetDRCBuffer(drcScanBuffer, scanBufferSize, (GX2DrcRenderMode) drcScanMode, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE);

    //! Setup color buffer for TV rendering
    GX2InitColorBuffer(&tvColorBuffer, GX2_SURFACE_DIM_TEXTURE_2D, tvWidth, tvHeight, 1, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, (GX2AAMode) tvAAMode);
    tvColorBuffer.surface.image = MEM1_alloc(tvColorBuffer.surface.imageSize, tvColorBuffer.surface.alignment);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, tvColorBuffer.surface.image, tvColorBuffer.surface.imageSize);

    //! due to AA we can only use 16 bit depth buffer in MEM1 otherwise we would have to switch to mem2 for depth buffer
    //! this should be ok for our purpose i guess

    //! Setup TV depth buffer (can be the same for both if rendered one after another)
    uint32_t size, align;
    GX2InitDepthBuffer(&tvDepthBuffer, GX2_SURFACE_DIM_TEXTURE_2D, tvColorBuffer.surface.width, tvColorBuffer.surface.height, 1, GX2_SURFACE_FORMAT_FLOAT_R32, (GX2AAMode) tvAAMode);
    tvDepthBuffer.surface.image = MEM1_alloc(tvDepthBuffer.surface.imageSize, tvDepthBuffer.surface.alignment);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, tvDepthBuffer.surface.image, tvDepthBuffer.surface.imageSize);

    //! Setup TV HiZ buffer
    GX2CalcDepthBufferHiZInfo(&tvDepthBuffer, &size, &align);
    tvDepthBuffer.hiZPtr = MEM1_alloc(size, align);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, tvDepthBuffer.hiZPtr, size);
    GX2InitDepthBufferHiZEnable(&tvDepthBuffer, GX2_ENABLE);

    //! Setup color buffer for DRC rendering
    GX2InitColorBuffer(&drcColorBuffer, GX2_SURFACE_DIM_TEXTURE_2D, 854, 480, 1, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, (GX2AAMode) drcAAMode);
    drcColorBuffer.surface.image = MEM1_alloc(drcColorBuffer.surface.imageSize, drcColorBuffer.surface.alignment);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, drcColorBuffer.surface.image, drcColorBuffer.surface.imageSize);

    //! Setup DRC depth buffer (can be the same for both if rendered one after another)
    GX2InitDepthBuffer(&drcDepthBuffer, GX2_SURFACE_DIM_TEXTURE_2D, drcColorBuffer.surface.width, drcColorBuffer.surface.height, 1, GX2_SURFACE_FORMAT_FLOAT_R32, (GX2AAMode) drcAAMode);
    drcDepthBuffer.surface.image = MEM1_alloc(drcDepthBuffer.surface.imageSize, drcDepthBuffer.surface.alignment);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, drcDepthBuffer.surface.image, drcDepthBuffer.surface.imageSize);

    //! Setup DRC HiZ buffer
    GX2CalcDepthBufferHiZInfo(&drcDepthBuffer, &size, &align);
    drcDepthBuffer.hiZPtr = MEM1_alloc(size, align);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, drcDepthBuffer.hiZPtr, size);
    GX2InitDepthBufferHiZEnable(&drcDepthBuffer, GX2_ENABLE);


    //! allocate auxilary buffer last as there might not be enough MEM1 left for other stuff after that
    if (tvColorBuffer.surface.aa) {
        uint32_t auxSize, auxAlign;
        GX2CalcColorBufferAuxInfo(&tvColorBuffer, &auxSize, &auxAlign);
        tvColorBuffer.aaBuffer = MEM1_alloc(auxSize, auxAlign);
        if (!tvColorBuffer.aaBuffer) {
            tvColorBuffer.aaBuffer = MEM2_alloc(auxSize, auxAlign);
        }

        tvColorBuffer.aaSize = auxSize;
        memset(tvColorBuffer.aaBuffer, GX2_AA_BUFFER_CLEAR_VALUE, auxSize);
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU, tvColorBuffer.aaBuffer, auxSize);
    }

    if (drcColorBuffer.surface.aa) {
        uint32_t auxSize, auxAlign;
        GX2CalcColorBufferAuxInfo(&drcColorBuffer, &auxSize, &auxAlign);
        drcColorBuffer.aaBuffer = MEM1_alloc(auxSize, auxAlign);
        if (!drcColorBuffer.aaBuffer) {
            drcColorBuffer.aaBuffer = MEM2_alloc(auxSize, auxAlign);
        }
        drcColorBuffer.aaSize = auxSize;
        memset(drcColorBuffer.aaBuffer, GX2_AA_BUFFER_CLEAR_VALUE, auxSize);
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU, drcColorBuffer.aaBuffer, auxSize);
    }

    //! allocate memory and setup context state TV
    tvContextState = (GX2ContextState *) MEM2_alloc(sizeof(GX2ContextState), GX2_CONTEXT_STATE_ALIGNMENT);
    GX2SetupContextStateEx(tvContextState, GX2_TRUE);

    //! allocate memory and setup context state DRC
    drcContextState = (GX2ContextState *) MEM2_alloc(sizeof(GX2ContextState), GX2_CONTEXT_STATE_ALIGNMENT);
    GX2SetupContextStateEx(drcContextState, GX2_TRUE);

    //! set initial context state and render buffers
    GX2SetContextState(tvContextState);
    GX2SetColorBuffer(&tvColorBuffer, GX2_RENDER_TARGET_0);
    GX2SetDepthBuffer(&tvDepthBuffer);

    GX2SetContextState(drcContextState);
    GX2SetColorBuffer(&drcColorBuffer, GX2_RENDER_TARGET_0);
    GX2SetDepthBuffer(&drcDepthBuffer);

    //! set initial viewport
    GX2SetViewport(0.0f, 0.0f, tvColorBuffer.surface.width, tvColorBuffer.surface.height, 0.0f, 1.0f);
    GX2SetScissor(0, 0, tvColorBuffer.surface.width, tvColorBuffer.surface.height);

    //! this is not necessary but can be used for swap counting and vsyncs
    GX2SetSwapInterval(1);

    //GX2SetTVGamma(0.8f);
    //GX2SetDRCGamma(0.8f);

    //! initialize perspective matrix
    const float cam_X_rot = 25.0f;

    projectionMtx = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);

    viewMtx = glm::mat4(1.0f);
    viewMtx = glm::translate(viewMtx, glm::vec3(0.0f, 0.0f, -2.5f));
    viewMtx = glm::rotate(viewMtx, DegToRad(cam_X_rot), glm::vec3(1.0f, 0.0f, 0.0f));

    GX2InitSampler(&aaSampler, GX2_TEX_CLAMP_MODE_CLAMP, GX2_TEX_XY_FILTER_MODE_LINEAR);
    GX2InitTexture(&tvAaTexture, tvColorBuffer.surface.width, tvColorBuffer.surface.height, 1, 0, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_SURFACE_DIM_TEXTURE_2D, GX2_TILE_MODE_DEFAULT);
    tvAaTexture.surface.image     = tvColorBuffer.surface.image;
    tvAaTexture.surface.imageSize = tvColorBuffer.surface.imageSize;
    tvAaTexture.surface.mipmaps   = tvColorBuffer.surface.mipmaps;
}

CVideo::~CVideo() {
    //! flush buffers
    GX2Flush();
    GX2DrawDone();
    //! shutdown
    GX2Shutdown();
    //! free command buffer memory
    MEM2_free(gx2CommandBuffer);
    //! free scan buffers
    MEMBucket_free(tvScanBuffer);
    MEMBucket_free(drcScanBuffer);
    //! free color buffers
    MEM1_free(tvColorBuffer.surface.image);
    MEM1_free(drcColorBuffer.surface.image);
    //! free depth buffers
    MEM1_free(tvDepthBuffer.surface.image);
    MEM1_free(tvDepthBuffer.hiZPtr);
    MEM1_free(drcDepthBuffer.surface.image);
    MEM1_free(drcDepthBuffer.hiZPtr);
    //! free context buffers
    MEM2_free(tvContextState);
    MEM2_free(drcContextState);
    //! free aux buffer
    if (tvColorBuffer.aaBuffer) {
        if (((uint32_t) tvColorBuffer.aaBuffer & 0xF0000000) == 0xF0000000) {
            MEM1_free(tvColorBuffer.aaBuffer);
        } else {
            MEM2_free(tvColorBuffer.aaBuffer);
        }
    }
    if (drcColorBuffer.aaBuffer) {
        if (((uint32_t) drcColorBuffer.aaBuffer & 0xF0000000) == 0xF0000000) {
            MEM1_free(drcColorBuffer.aaBuffer);
        } else {
            MEM2_free(drcColorBuffer.aaBuffer);
        }
    }
    //! destroy shaders
    ColorShader::destroyInstance();
    FXAAShader::destroyInstance();
    Shader3D::destroyInstance();
    ShaderFractalColor::destroyInstance();
    Texture2DShader::destroyInstance();
}

void CVideo::renderFXAA(const GX2Texture *texture, const GX2Sampler *sampler) {
    resolution[0] = texture->surface.width;
    resolution[1] = texture->surface.height;

    GX2Invalidate((GX2InvalidateMode) (GX2_INVALIDATE_MODE_COLOR_BUFFER | GX2_INVALIDATE_MODE_TEXTURE), texture->surface.image, texture->surface.imageSize);

    GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_ALWAYS);
    FXAAShader::instance()->setShaders();
    FXAAShader::instance()->setAttributeBuffer();
    FXAAShader::instance()->setResolution(resolution);
    FXAAShader::instance()->setTextureAndSampler(texture, sampler);
    FXAAShader::instance()->draw();
    GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LEQUAL);
}
