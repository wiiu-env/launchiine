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
#ifndef _GUI_PARTICLE_IMAGE_H_
#define _GUI_PARTICLE_IMAGE_H_

#include <gui/GuiImage.h>

class GuiParticleImage : public GuiImage, public sigslot::has_slots<> {
public:
    GuiParticleImage(int32_t w, int32_t h, uint32_t particleCount, float minRadius, float maxRadius, float minSpeed, float maxSpeed);

    ~GuiParticleImage() override;

    void draw(const CVideo &pVideo) override;

private:
    float *posVertexs;
    uint8_t *colorVertexs;

    float minRadius;
    float maxRadius;
    float minSpeed;
    float maxSpeed;

    typedef struct {
        glm::vec3 position;
        glm::vec4 colors;
        float radius;
        float speed;
        float direction;
    } Particle;

    std::vector<Particle> particles;
};

#endif // _GUI_ICON_GRID_H_
