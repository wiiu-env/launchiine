/****************************************************************************
 * Copyright (C) 2016,2017 Maschell
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <video/CursorDrawer.h>
#include <video/shaders/ColorShader.h>
#include <video/shaders/FXAAShader.h>
#include <video/shaders/Shader3D.h>
#include <video/shaders/ShaderFractalColor.h>
#include <video/shaders/Texture2DShader.h>

CursorDrawer *CursorDrawer::instance = NULL;

CursorDrawer::CursorDrawer() {
    init_colorVtxs();
}

CursorDrawer::~CursorDrawer() {
    //! destroy shaders
    ColorShader::destroyInstance();
    FXAAShader::destroyInstance();
    Shader3D::destroyInstance();
    ShaderFractalColor::destroyInstance();
    Texture2DShader::destroyInstance();
    if (this->colorVtxs) {
        free(this->colorVtxs);
        this->colorVtxs = NULL;
    }
}

void CursorDrawer::init_colorVtxs() {
    if (!this->colorVtxs) {
        this->colorVtxs = (uint8_t *) memalign(0x40, sizeof(uint8_t) * 16);
        if (this->colorVtxs == NULL) { return; }
    }
    memset(this->colorVtxs, 0xFF, 16 * sizeof(uint8_t));

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, this->colorVtxs, 16 * sizeof(uint8_t));
}

// Could be improved. It be more generic.
void CursorDrawer::draw_Cursor(float x, float y) {
    if (this->colorVtxs == NULL) {
        init_colorVtxs();
        return;
    }

    float widthScaleFactor  = 1.0f / (float) 1280;
    float heightScaleFactor = 1.0f / (float) 720;

    int32_t width = 20;

    glm::vec3 positionOffsets = glm::vec3(0.0f);

    positionOffsets[0] = (x - ((1280) / 2) + (width / 2)) * widthScaleFactor * 2.0f;
    positionOffsets[1] = -(y - ((720) / 2) + (width / 2)) * heightScaleFactor * 2.0f;

    glm::vec3 scale(width * widthScaleFactor, width * heightScaleFactor, 1.0f);

    ColorShader::instance()->setShaders();
    ColorShader::instance()->setAttributeBuffer(this->colorVtxs, NULL, 4);
    ColorShader::instance()->setAngle(0);
    ColorShader::instance()->setOffset(positionOffsets);
    ColorShader::instance()->setScale(scale);
    ColorShader::instance()->setColorIntensity(glm::vec4(1.0f));
    ColorShader::instance()->draw(GX2_PRIMITIVE_MODE_QUADS, 4);
}
