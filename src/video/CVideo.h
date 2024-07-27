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
#ifndef __CVIDEO_H_
#define __CVIDEO_H_

#include <gui/gx2_ext.h>
#include <gx2/clear.h>
#include <gx2/context.h>
#include <gx2/display.h>
#include <gx2/draw.h>
#include <gx2/event.h>
#include <gx2/registers.h>
#include <gx2/sampler.h>
#include <gx2/state.h>
#include <gx2/swap.h>

#include <video/shaders/Shader.h>

class CVideo {
public:
    CVideo(int32_t forceTvScanMode = -1, int32_t forceDrcScanMode = -1);

    virtual ~CVideo();

    void prepareTvRendering(void) {
        currContextState = tvContextState;
        currColorBuffer  = &tvColorBuffer;
        currDepthBuffer  = &tvDepthBuffer;
        prepareRendering();
    }

    void prepareDrcRendering(void) {
        currContextState = drcContextState;
        currColorBuffer  = &drcColorBuffer;
        currDepthBuffer  = &drcDepthBuffer;
        prepareRendering();
    }

    void prepareRendering(void) {
        GX2ClearColor(currColorBuffer, 0.0f, 0.0f, 0.0f, 1.0f);
        GX2ClearDepthStencilEx(currDepthBuffer, currDepthBuffer->depthClear, currDepthBuffer->stencilClear, GX2_CLEAR_FLAGS_BOTH);

        GX2SetContextState(currContextState);
        GX2SetViewport(0.0f, 0.0f, currColorBuffer->surface.width, currColorBuffer->surface.height, 0.0f, 1.0f);
        GX2SetScissor(0, 0, currColorBuffer->surface.width, currColorBuffer->surface.height);

        GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LEQUAL);
        GX2SetColorControl(GX2_LOGIC_OP_COPY, 1, GX2_DISABLE, GX2_ENABLE);
        GX2SetBlendControl(GX2_RENDER_TARGET_0, GX2_BLEND_MODE_SRC_ALPHA, GX2_BLEND_MODE_INV_SRC_ALPHA, GX2_BLEND_COMBINE_MODE_ADD, GX2_ENABLE, GX2_BLEND_MODE_SRC_ALPHA, GX2_BLEND_MODE_INV_SRC_ALPHA, GX2_BLEND_COMBINE_MODE_ADD);
        GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_DISABLE, GX2_ENABLE);
    }

    void setStencilRender(bool bEnable) {
        if (bEnable) {
            GX2SetStencilMask(0xff, 0xff, 0x01, 0xff, 0xff, 0x01);
            GX2SetDepthStencilControl(GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_LEQUAL, GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_ALWAYS, GX2_STENCIL_FUNCTION_KEEP, GX2_STENCIL_FUNCTION_KEEP, GX2_STENCIL_FUNCTION_REPLACE,
                                      GX2_COMPARE_FUNC_ALWAYS, GX2_STENCIL_FUNCTION_KEEP, GX2_STENCIL_FUNCTION_KEEP, GX2_STENCIL_FUNCTION_REPLACE);
        } else {
            GX2SetStencilMask(0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
            GX2SetDepthStencilControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LEQUAL, GX2_DISABLE, GX2_DISABLE, GX2_COMPARE_FUNC_NEVER, GX2_STENCIL_FUNCTION_KEEP, GX2_STENCIL_FUNCTION_KEEP, GX2_STENCIL_FUNCTION_KEEP,
                                      GX2_COMPARE_FUNC_NEVER, GX2_STENCIL_FUNCTION_KEEP, GX2_STENCIL_FUNCTION_KEEP, GX2_STENCIL_FUNCTION_KEEP);
        }
    }

    void drcDrawDone(void) {
        //! on DRC we do a hardware AA because FXAA does not look good
        //renderFXAA(&drcAaTexture, &aaSampler);
        GX2CopyColorBufferToScanBuffer(&drcColorBuffer, GX2_SCAN_TARGET_DRC);
    }

    void tvDrawDone(void) {
        renderFXAA(&tvAaTexture, &aaSampler);
        GX2CopyColorBufferToScanBuffer(&tvColorBuffer, GX2_SCAN_TARGET_TV);
        GX2SwapScanBuffers();
        GX2Flush();
    }

    void waitForVSync(void) {
        GX2WaitForVsync();
        frameCount++;
    }

    void tvEnable(bool bEnable) {
        if (tvEnabled != bEnable) {
            GX2SetTVEnable(bEnable ? GX2_ENABLE : GX2_DISABLE);
            tvEnabled = bEnable;
        }
    }

    void drcEnable(bool bEnable) {
        if (drcEnabled != bEnable) {
            GX2SetDRCEnable(bEnable ? GX2_ENABLE : GX2_DISABLE);
            drcEnabled = bEnable;
        }
    }

    uint32_t getFrameCount(void) const {
        return frameCount;
    }

    uint32_t getTvWidth(void) const {
        return tvColorBuffer.surface.width;
    }

    uint32_t getTvHeight(void) const {
        return tvColorBuffer.surface.height;
    }

    uint32_t getDrcWidth(void) const {
        return drcColorBuffer.surface.width;
    }

    uint32_t getDrcHeight(void) const {
        return drcColorBuffer.surface.height;
    }

    const glm::mat4 &getProjectionMtx(void) const {
        return projectionMtx;
    }

    const glm::mat4 &getViewMtx(void) const {
        return viewMtx;
    }

    float getWidthScaleFactor(void) const {
        return widthScaleFactor;
    }

    float getHeightScaleFactor(void) const {
        return heightScaleFactor;
    }

    float getDepthScaleFactor(void) const {
        return depthScaleFactor;
    }

    void screenPosToWorldRay(float posX, float posY, glm::vec3 &rayOrigin, glm::vec3 &rayDirection) {
        //! normalize positions
        posX = 2.0f * posX * getWidthScaleFactor();
        posY = 2.0f * posY * getHeightScaleFactor();

        glm::vec4 rayStart(posX, posY, 0.0f, 1.0f);
        glm::vec4 rayEnd(posX, posY, 1.0f, 1.0f);

        glm::mat4 IMV           = glm::inverse(projectionMtx * viewMtx);
        glm::vec4 rayStartWorld = IMV * rayStart;
        rayStartWorld /= rayStartWorld.w;

        glm::vec4 rayEndWorld = IMV * rayEnd;
        rayEndWorld /= rayEndWorld.w;

        glm::vec3 rayDirectionWorld(rayEndWorld - rayStartWorld);
        rayDirectionWorld = glm::normalize(rayDirectionWorld);

        rayOrigin    = glm::vec3(rayStartWorld);
        rayDirection = glm::normalize(rayDirectionWorld);
    }

private:
    static void *GX2RAlloc(uint32_t flags, uint32_t size, uint32_t align);

    static void GX2RFree(uint32_t flags, void *p);

    void renderFXAA(const GX2Texture *texture, const GX2Sampler *sampler);

    void *gx2CommandBuffer;

    void *tvScanBuffer;
    void *drcScanBuffer;

    uint32_t frameCount;
    float widthScaleFactor;
    float heightScaleFactor;
    float depthScaleFactor;

    bool tvEnabled;
    bool drcEnabled;

    GX2ColorBuffer tvColorBuffer;
    GX2DepthBuffer tvDepthBuffer;
    GX2ColorBuffer drcColorBuffer;
    GX2DepthBuffer drcDepthBuffer;

    GX2ContextState *tvContextState;
    GX2ContextState *drcContextState;

    GX2ContextState *currContextState;
    GX2ColorBuffer *currColorBuffer;
    GX2DepthBuffer *currDepthBuffer;

    GX2Texture tvAaTexture;
    GX2Sampler aaSampler;

    glm::mat4 projectionMtx;
    glm::mat4 viewMtx;
    glm::vec2 resolution;
};

#endif // __GX2_VIDEO_H_
