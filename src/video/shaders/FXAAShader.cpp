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
#include <video/shaders/FXAAShader.h>
#include <malloc.h>
#include <string.h>

static const uint32_t cpVertexShaderProgram[] = {
        0x00000000, 0x00008009, 0x20000000, 0x000004a0,
        0x3ca00000, 0x88060094, 0x00400000, 0xff0f2094,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0xfd001f80, 0x900c2060, 0x0000803f, 0x00000000,
        0xc1a229f5, 0xd0eddc33, 0x426618fd, 0x8509cfe7};

static const uint32_t cpVertexShaderRegs[] = {
        0x00000102, 0x00000000, 0x00000000, 0x00000001,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0x00000000, 0xfffffffe,
        0x00000001, 0x00000000, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x00000000, 0x0000000e, 0x00000010};

static const uint32_t cpPixelShaderProgram[] = {
        0x20000000, 0x00003ca0, 0xa0000000, 0x000c8080,
        0x30000000, 0x000010a1, 0xa8000000, 0x0010c080,
        0x75000000, 0x000088a0, 0x00800100, 0x88062094,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00241f02, 0x1000e00f, 0x00241f00, 0x1000e02f,
        0x00201f02, 0x00000040, 0x00201f00, 0x00000060,
        0x00011f80, 0x10332060, 0xff000000, 0xff102200,
        0xfd001f00, 0x900cc020, 0xffc09f01, 0x90004040,
        0xffc01f01, 0x90000060, 0x00051f80, 0x1033a040,
        0x0000803f, 0x00000000, 0xffe00f00, 0x90004000,
        0xff008000, 0xff102220, 0xffe08f00, 0x90000440,
        0x010c0000, 0x010c4660, 0xff008080, 0xff004220,
        0x01a01f00, 0x00280000, 0x01a49f00, 0x00280020,
        0x01a81f01, 0x00280040, 0xfd0c1f00, 0x1028e06f,
        0x00208081, 0x90002000, 0x8716993e, 0xa245163f,
        0xd578e93d, 0x00000080, 0x03a01f00, 0x00280000,
        0x03a49f00, 0x00280020, 0x03a81f01, 0x1028e04f,
        0xfd0c1f00, 0x00280060, 0x00a40081, 0x90002020,
        0x8716993e, 0xa245163f, 0xd578e93d, 0x00000080,
        0x04a01f00, 0x00280000, 0x04a49f00, 0x1028a02f,
        0x04a81f01, 0x00280040, 0xfd0c1f00, 0x00280060,
        0x7fcc1f80, 0x1000c02f, 0x8716993e, 0xa245163f,
        0xd578e93d, 0x00000080, 0x02a01f00, 0x1028e00f,
        0x02a49f00, 0x00280020, 0x02a81f01, 0x00280040,
        0xfd0c1f00, 0x00280060, 0x7fcc1f80, 0x1000e02f,
        0x8716993e, 0xa245163f, 0xd578e93d, 0x00000080,
        0x7dc41f00, 0x00020000, 0x7fec0f01, 0x00020020,
        0x7fc81f00, 0x00000040, 0x7dc41f00, 0x00000060,
        0x7fec0f81, 0x9001802f, 0xfef88f00, 0x1000e00f,
        0xfedc8f00, 0x00000420, 0x7de40f00, 0x80010040,
        0x7ec49f01, 0x00001060, 0xfec41f80, 0x10024060,
        0xfed49f00, 0x80020000, 0xfe141f00, 0x900c802f,
        0xfeac1f00, 0x80000040, 0xfec01f02, 0x80020060,
        0x7cc41f81, 0x90010060, 0x0000003d, 0x00000000,
        0xfd001f00, 0x900c6000, 0xfea89f00, 0x80010020,
        0xfec09f81, 0x00020040, 0x0000803f, 0x0000003e,
        0xfec41f81, 0x00000020, 0xfe041f80, 0x00330000,
        0x7fe01f00, 0x80000040, 0x7ce41f80, 0x80000060,
        0xfea81f00, 0x80010000, 0xfeac1f80, 0x80010020,
        0x000000c1, 0x00000000, 0xfea01f00, 0x00020040,
        0xfea41f80, 0x00020060, 0x00000041, 0x00000000,
        0x05c81f01, 0x9000e00f, 0x01cc9f81, 0x9000e06f,
        0xfeac1f00, 0x01004200, 0xfea01f00, 0x01044220,
        0xfeac9f00, 0x01002240, 0xfea09f00, 0x01042260,
        0xfe8c1f80, 0x01008600, 0xacaa2a3e, 0xaaaa2abe,
        0x7f9c1f00, 0x0100a200, 0x7f801f00, 0x01048220,
        0x7f901f80, 0x0104a240, 0x02080001, 0x7000a00f,
        0x02000000, 0x7000c04f, 0x02048000, 0x7000e06f,
        0x01a81f80, 0x9000e00f, 0xd578e93d, 0x00000000,
        0x04a80001, 0x1000c00f, 0x04a48000, 0x00000020,
        0x04a00000, 0x00000040, 0xfe081f00, 0xe00c0060,
        0xfe0c1f80, 0xe00c0000, 0x01a41f00, 0x7f00620f,
        0xfea89f00, 0xfe0c822f, 0xfea49f00, 0xff00a24f,
        0x7d001f80, 0xe00c0060, 0xa245163f, 0x0000803e,
        0x7ea01f00, 0xfe0ce20f, 0x01a09f80, 0xfe006a4f,
        0x0000803e, 0x8716993e, 0xfe088001, 0x9001c00f,
        0xfe488001, 0x1002e44f, 0xfea01f80, 0x80000000,
        0xd578e93d, 0x00000000, 0x7ca41f00, 0x00280000,
        0x7da89f00, 0x00280020, 0xff201f00, 0x00280040,
        0xfd081f80, 0x00280060, 0x8716993e, 0xa245163f,
        0x00000080, 0x00000000, 0x7fc81f00, 0x80060000,
        0xfec00f80, 0x80060060, 0xfec09f81, 0xfb80634f,
        0xfe888f00, 0x7e886300, 0xfea80f01, 0x7f8c6320,
        0xfee80f00, 0x7d806340, 0xfe680080, 0x06846f60,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x10000100, 0x01101df0, 0x00008010, 0xecdfea0d,
        0x10000200, 0x03101df0, 0x00002050, 0xecdfea0d,
        0x10000000, 0x04101df0, 0x00003071, 0xecdfea0d,
        0x10000200, 0x02101df0, 0x0000b070, 0xecdfea0d,
        0x10000200, 0x02101df0, 0x00008010, 0xecdfea0d,
        0x10000100, 0x00101df0, 0x0000a051, 0xecdfea0d,
        0x10000400, 0x04101df0, 0x00008010, 0xecdfea0d,
        0x10000500, 0x05101df0, 0x00000011, 0xecdfea0d,
        0x10000100, 0x01101df0, 0x00008010, 0xecdfea0d,
        0xfe2e963a, 0x0269a9a3, 0x38f88096, 0x400cf48b};
static const uint32_t cpPixelShaderRegs[] = {
        0x00000007, 0x00000002, 0x04000101, 0x00000000,
        0x00000001, 0x00000100, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x0000000f, 0x00000001, 0x00000010,
        0x00000000};

FXAAShader *FXAAShader::shaderInstance = NULL;

FXAAShader::FXAAShader()
    : vertexShader(cuAttributeCount) {
    //! create pixel shader
    pixelShader.setProgram(cpPixelShaderProgram, sizeof(cpPixelShaderProgram), cpPixelShaderRegs, sizeof(cpPixelShaderRegs));

    resolutionLocation = 0;
    pixelShader.addUniformVar((GX2UniformVar){
            "unf_resolution", GX2_SHADER_VAR_TYPE_FLOAT2, 1, resolutionLocation, -1});

    samplerLocation = 0;
    pixelShader.addSamplerVar((GX2SamplerVar){
            "sampl_texture", GX2_SAMPLER_VAR_TYPE_SAMPLER_2D, samplerLocation});

    //! create vertex shader
    vertexShader.setProgram(cpVertexShaderProgram, sizeof(cpVertexShaderProgram), cpVertexShaderRegs, sizeof(cpVertexShaderRegs));

    positionLocation = 0;
    texCoordLocation = 1;
    vertexShader.addAttribVar((GX2AttribVar){
            "attr_position", GX2_SHADER_VAR_TYPE_FLOAT3, 0, positionLocation});
    vertexShader.addAttribVar((GX2AttribVar){
            "attr_texture_coord", GX2_SHADER_VAR_TYPE_FLOAT2, 0, texCoordLocation});

    //! setup attribute streams
    GX2InitAttribStream(vertexShader.getAttributeBuffer(0), positionLocation, 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32);
    GX2InitAttribStream(vertexShader.getAttributeBuffer(1), texCoordLocation, 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);

    //! create fetch shader
    fetchShader = new FetchShader(vertexShader.getAttributeBuffer(), vertexShader.getAttributesCount());

    //! model vertex has to be align and cannot be in unknown regions for GX2 like 0xBCAE1000
    posVtxs   = (float *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, ciPositionVtxsSize);
    texCoords = (float *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, ciTexCoordsVtxsSize);

    //! position vertex structure and texture coordinate vertex structure
    int32_t i    = 0;
    posVtxs[i++] = -1.0f;
    posVtxs[i++] = -1.0f;
    posVtxs[i++] = 0.0f;
    posVtxs[i++] = 1.0f;
    posVtxs[i++] = -1.0f;
    posVtxs[i++] = 0.0f;
    posVtxs[i++] = 1.0f;
    posVtxs[i++] = 1.0f;
    posVtxs[i++] = 0.0f;
    posVtxs[i++] = -1.0f;
    posVtxs[i++] = 1.0f;
    posVtxs[i++] = 0.0f;
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, posVtxs, ciPositionVtxsSize);

    i              = 0;
    texCoords[i++] = 0.0f;
    texCoords[i++] = 1.0f;
    texCoords[i++] = 1.0f;
    texCoords[i++] = 1.0f;
    texCoords[i++] = 1.0f;
    texCoords[i++] = 0.0f;
    texCoords[i++] = 0.0f;
    texCoords[i++] = 0.0f;
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, texCoords, ciTexCoordsVtxsSize);
}

FXAAShader::~FXAAShader() {
    if (posVtxs) {
        free(posVtxs);
        posVtxs = NULL;
    }
    if (texCoords) {
        free(texCoords);
        texCoords = NULL;
    }

    delete fetchShader;
    fetchShader = NULL;
}
