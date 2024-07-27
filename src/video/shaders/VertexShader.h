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
#ifndef VERTEX_SHADER_H
#define VERTEX_SHADER_H

#include <gui/gx2_ext.h>
#include <video/shaders/Shader.h>
#include <string.h>

class VertexShader : public Shader {
public:
    VertexShader(uint32_t numAttr)
        : attributesCount(numAttr), attributes(new GX2AttribStream[attributesCount]), vertexShader((GX2VertexShader *) memalign(0x40, sizeof(GX2VertexShader))) {
        if (vertexShader) {
            memset(vertexShader, 0, sizeof(GX2VertexShader));
            vertexShader->mode = GX2_SHADER_MODE_UNIFORM_REGISTER;
        }
    }

    virtual ~VertexShader() {
        delete[] attributes;

        if (vertexShader) {
            if (vertexShader->program) {
                free(vertexShader->program);
            }

            for (uint32_t i = 0; i < vertexShader->uniformBlockCount; i++) {
                free((void *) vertexShader->uniformBlocks[i].name);
            }

            if (vertexShader->uniformBlocks) {
                free((void *) vertexShader->uniformBlocks);
            }

            for (uint32_t i = 0; i < vertexShader->uniformVarCount; i++) {
                free((void *) vertexShader->uniformVars[i].name);
            }

            if (vertexShader->uniformVars) {
                free((void *) vertexShader->uniformVars);
            }

            if (vertexShader->initialValues) {
                free((void *) vertexShader->initialValues);
            }

            for (uint32_t i = 0; i < vertexShader->samplerVarCount; i++) {
                free((void *) vertexShader->samplerVars[i].name);
            }

            if (vertexShader->samplerVars) {
                free((void *) vertexShader->samplerVars);
            }

            for (uint32_t i = 0; i < vertexShader->attribVarCount; i++) {
                free((void *) vertexShader->attribVars[i].name);
            }

            if (vertexShader->attribVars) {
                free((void *) vertexShader->attribVars);
            }

            if (vertexShader->loopVars) {
                free((void *) vertexShader->loopVars);
            }

            free(vertexShader);
        }
    }

    void setProgram(const uint32_t *program, const uint32_t &programSize, const uint32_t *regs, const uint32_t &regsSize) {
        if (!vertexShader) {
            return;
        }

        //! this must be moved into an area where the graphic engine has access to and must be aligned to 0x100
        vertexShader->size    = programSize;
        vertexShader->program = (uint8_t *) memalign(GX2_SHADER_PROGRAM_ALIGNMENT, vertexShader->size);
        if (vertexShader->program) {
            memcpy(vertexShader->program, program, vertexShader->size);
            GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, vertexShader->program, vertexShader->size);
        }

        memcpy(&vertexShader->regs, regs, regsSize);
    }

    void addUniformVar(const GX2UniformVar &var) {
        if (!vertexShader) {
            return;
        }

        uint32_t idx = vertexShader->uniformVarCount;

        GX2UniformVar *newVar = (GX2UniformVar *) malloc((vertexShader->uniformVarCount + 1) * sizeof(GX2UniformVar));
        if (newVar) {
            if (vertexShader->uniformVarCount > 0) {
                memcpy(newVar, vertexShader->uniformVars, vertexShader->uniformVarCount * sizeof(GX2UniformVar));
                free(vertexShader->uniformVars);
            }
            vertexShader->uniformVars = newVar;

            memcpy(vertexShader->uniformVars + idx, &var, sizeof(GX2UniformVar));
            vertexShader->uniformVars[idx].name = (char *) malloc(strlen(var.name) + 1);
            strcpy((char *) vertexShader->uniformVars[idx].name, var.name);

            vertexShader->uniformVarCount++;
        }
    }

    void addAttribVar(const GX2AttribVar &var) {
        if (!vertexShader) {
            return;
        }

        uint32_t idx = vertexShader->attribVarCount;

        GX2AttribVar *newVar = (GX2AttribVar *) malloc((vertexShader->attribVarCount + 1) * sizeof(GX2AttribVar));
        if (newVar) {
            if (vertexShader->attribVarCount > 0) {
                memcpy(newVar, vertexShader->attribVars, vertexShader->attribVarCount * sizeof(GX2AttribVar));
                free(vertexShader->attribVars);
            }
            vertexShader->attribVars = newVar;

            memcpy(vertexShader->attribVars + idx, &var, sizeof(GX2AttribVar));
            vertexShader->attribVars[idx].name = (char *) malloc(strlen(var.name) + 1);
            strcpy((char *) vertexShader->attribVars[idx].name, var.name);

            vertexShader->attribVarCount++;
        }
    }

    static inline void setAttributeBuffer(uint32_t bufferIdx, uint32_t bufferSize, uint32_t stride, const void *buffer) {
        GX2SetAttribBuffer(bufferIdx, bufferSize, stride, (void *) buffer);
    }

    GX2VertexShader *getVertexShader() const {
        return vertexShader;
    }

    void setShader(void) const {
        GX2SetVertexShader(vertexShader);
    }

    GX2AttribStream *getAttributeBuffer(uint32_t idx = 0) const {
        if (idx >= attributesCount) {
            return NULL;
        }
        return &attributes[idx];
    }

    uint32_t getAttributesCount() const {
        return attributesCount;
    }

    static void setUniformReg(uint32_t location, uint32_t size, const void *reg) {
        GX2SetVertexUniformReg(location, size, (uint32_t *) reg);
    }

protected:
    uint32_t attributesCount;
    GX2AttribStream *attributes;
    GX2VertexShader *vertexShader;
};

#endif // VERTEX_SHADER_H
