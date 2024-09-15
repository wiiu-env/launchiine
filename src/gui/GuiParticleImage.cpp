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
#include <gui/GuiParticleImage.h>
#include <video/CVideo.h>
#include <video/shaders/ColorShader.h>

#define CIRCLE_VERTEX_COUNT 36

static inline float getRandZeroToOneF32() {
    return (rand() % 10000) * 0.0001f;
}

static inline float getRandMinusOneToOneF32() {
    return getRandZeroToOneF32() * 2.0f - 1.0f;
}

GuiParticleImage::GuiParticleImage(int32_t w, int32_t h, uint32_t particleCount, float minRadius, float maxRadius, float minSpeed, float maxSpeed)
    : GuiImage(nullptr) {
    width           = w;
    height          = h;
    imgType         = IMAGE_COLOR;
    this->minRadius = minRadius;
    this->maxRadius = maxRadius;
    this->minSpeed  = minSpeed;
    this->maxSpeed  = maxSpeed;

    posVertexs = (float *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, ColorShader::cuVertexAttrSize * CIRCLE_VERTEX_COUNT);
    if (!posVertexs) {
        throw std::runtime_error("Failed to allocate posVertexs in GuiParticleImage");
    }
    colorVertexs = (uint8_t *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, ColorShader::cuColorAttrSize * CIRCLE_VERTEX_COUNT);
    if (!colorVertexs) {
        throw std::runtime_error("Failed to allocate colorVertexs in GuiParticleImage");
    }

    for (uint32_t i = 0; i < CIRCLE_VERTEX_COUNT; i++) {
        posVertexs[i * 3 + 0] = cosf(DegToRad(i * 360.0f / CIRCLE_VERTEX_COUNT));
        posVertexs[i * 3 + 1] = sinf(DegToRad(i * 360.0f / CIRCLE_VERTEX_COUNT));
        posVertexs[i * 3 + 2] = 0.0f;

        colorVertexs[i * 4 + 0] = 0xff;
        colorVertexs[i * 4 + 1] = 0xff;
        colorVertexs[i * 4 + 2] = 0xff;
        colorVertexs[i * 4 + 3] = 0xff;
    }
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, posVertexs, ColorShader::cuVertexAttrSize * CIRCLE_VERTEX_COUNT);
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, colorVertexs, ColorShader::cuColorAttrSize * CIRCLE_VERTEX_COUNT);

    particles.resize(particleCount);

    for (uint32_t i = 0; i < particleCount; i++) {
        particles[i].position.x = getRandMinusOneToOneF32() * getWidth() * 0.5f;
        particles[i].position.y = getRandMinusOneToOneF32() * getHeight() * 0.5f;
        particles[i].position.z = 0.0f;
        particles[i].colors     = glm::vec4(1.0f, 1.0f, 1.0f, (getRandZeroToOneF32() * 0.6f) + 0.05f);
        particles[i].radius     = getRandZeroToOneF32() * (maxRadius - minRadius) + minRadius;
        particles[i].speed      = (getRandZeroToOneF32() * (maxSpeed - minSpeed)) + minSpeed;
        particles[i].direction  = getRandMinusOneToOneF32();
    }
}

GuiParticleImage::~GuiParticleImage() {
    free(posVertexs);
    free(colorVertexs);
}

void GuiParticleImage::draw(const CVideo &pVideo) {
    if (!this->isVisible()) {
        return;
    }

    float currScaleX = getScaleX();
    float currScaleY = getScaleY();

    positionOffsets[2] = getDepth() * pVideo.getDepthScaleFactor() * 2.0f;

    scaleFactor[2] = getScaleZ();

    //! add other colors intensities parameters
    colorIntensity[3] = getAlpha();

    for (auto &particle : particles) {
        if (particle.position.y > (getHeight() * 0.5f + 30.0f)) {
            particle.position.x = getRandMinusOneToOneF32() * getWidth() * 0.5f;
            particle.position.y = -getHeight() * 0.5f - 30.0f;
            particle.colors     = glm::vec4(1.0f, 1.0f, 1.0f, (getRandZeroToOneF32() * 0.6f) + 0.05f);
            particle.radius     = getRandZeroToOneF32() * (maxRadius - minRadius) + minRadius;
            particle.speed      = (getRandZeroToOneF32() * (maxSpeed - minSpeed)) + minSpeed;
            particle.direction  = getRandMinusOneToOneF32();
        }
        if (particle.position.x < (-getWidth() * 0.5f - 50.0f)) {
            particle.position.x = -particle.position.x;
        }

        particle.direction += getRandMinusOneToOneF32() * 0.03f;
        particle.position.x += particle.speed * particle.direction;
        particle.position.y += particle.speed;

        positionOffsets[0] = (getCenterX() + particle.position.x) * pVideo.getWidthScaleFactor() * 2.0f;
        positionOffsets[1] = (getCenterY() + particle.position.y) * pVideo.getHeightScaleFactor() * 2.0f;

        scaleFactor[0] = currScaleX * particle.radius * pVideo.getWidthScaleFactor();
        scaleFactor[1] = currScaleY * particle.radius * pVideo.getHeightScaleFactor();

        ColorShader::instance()->setShaders();
        ColorShader::instance()->setAttributeBuffer(colorVertexs, posVertexs, CIRCLE_VERTEX_COUNT);
        ColorShader::instance()->setAngle(0.0f);
        ColorShader::instance()->setOffset(positionOffsets);
        ColorShader::instance()->setScale(scaleFactor);
        ColorShader::instance()->setColorIntensity(colorIntensity * particle.colors);
        ColorShader::instance()->draw(GX2_PRIMITIVE_MODE_TRIANGLE_FAN, CIRCLE_VERTEX_COUNT);
    }
}
