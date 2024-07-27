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
#ifndef SHADER_3D_H_
#define SHADER_3D_H_

#include <video/shaders/FetchShader.h>
#include <video/shaders/PixelShader.h>
#include <video/shaders/VertexShader.h>

class Shader3D : public Shader {
private:
    Shader3D();

    virtual ~Shader3D();

    static Shader3D *shaderInstance;

    static const unsigned char cuAttributeCount = 2;
    static const uint32_t ciPositionVtxsSize    = 4 * cuVertexAttrSize;
    static const uint32_t ciTexCoordsVtxsSize   = 4 * cuTexCoordAttrSize;

    FetchShader *fetchShader;
    VertexShader vertexShader;
    PixelShader pixelShader;

    float *posVtxs;
    float *texCoords;

    uint32_t modelMatrixLocation;
    uint32_t viewMatrixLocation;
    uint32_t projectionMatrixLocation;
    uint32_t positionLocation;
    uint32_t texCoordLocation;

    uint32_t colorIntensityLocation;
    uint32_t fadeDistanceLocation;
    uint32_t fadeOutLocation;
    uint32_t samplerLocation;

public:
    static Shader3D *instance() {
        if (!shaderInstance) {
            shaderInstance = new Shader3D();
        }
        return shaderInstance;
    }

    static void destroyInstance() {
        if (shaderInstance) {
            delete shaderInstance;
            shaderInstance = NULL;
        }
    }

    void setShaders(void) const {
        fetchShader->setShader();
        vertexShader.setShader();
        pixelShader.setShader();
    }

    void setAttributeBuffer(const uint32_t &vtxCount = 0, const float *posVtxs_in = NULL, const float *texCoords_in = NULL) const {
        if (posVtxs_in && texCoords_in && vtxCount) {
            VertexShader::setAttributeBuffer(0, vtxCount * cuVertexAttrSize, cuVertexAttrSize, posVtxs_in);
            VertexShader::setAttributeBuffer(1, vtxCount * cuTexCoordAttrSize, cuTexCoordAttrSize, texCoords_in);
        } else {
            //! use default quad vertex and texture coordinates if nothing is passed
            VertexShader::setAttributeBuffer(0, ciPositionVtxsSize, cuVertexAttrSize, posVtxs);
            VertexShader::setAttributeBuffer(1, ciTexCoordsVtxsSize, cuTexCoordAttrSize, texCoords);
        }
    }

    void setProjectionMtx(const glm::mat4 &mtx) {
        VertexShader::setUniformReg(projectionMatrixLocation, 16, &mtx[0][0]);
    }

    void setViewMtx(const glm::mat4 &mtx) {
        VertexShader::setUniformReg(viewMatrixLocation, 16, &mtx[0][0]);
    }

    void setModelViewMtx(const glm::mat4 &mtx) {
        VertexShader::setUniformReg(modelMatrixLocation, 16, &mtx[0][0]);
    }

    void setColorIntensity(const glm::vec4 &vec) {
        PixelShader::setUniformReg(colorIntensityLocation, 4, &vec[0]);
    }

    void setAlphaFadeOut(const glm::vec4 &vec) {
        PixelShader::setUniformReg(fadeOutLocation, 4, &vec[0]);
    }

    void setDistanceFadeOut(const float &value) {
        PixelShader::setUniformReg(fadeDistanceLocation, 4, &value);
    }

    void setTextureAndSampler(const GX2Texture *texture, const GX2Sampler *sampler) const {
        GX2SetPixelTexture((GX2Texture *) texture, samplerLocation);
        GX2SetPixelSampler((GX2Sampler *) sampler, samplerLocation);
    }
};

#endif // SHADER_3D_H_
