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
#include <malloc.h>
#include <string.h>
#include <video/shaders/Texture2DShader.h>

static const uint32_t cpVertexShaderProgram[] = {
        0x00000000, 0x00008009, 0x20000000, 0x000080a0,
        0x3c200100, 0x88060094, 0x00400000, 0x88042014,
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
        0x02290001, 0x80000000, 0x02041f00, 0x900c0020,
        0x00a11f00, 0xfc00624f, 0xfd041f00, 0x900c4060,
        0x02001f80, 0x900c0000, 0x83f9223e, 0x0000803f,
        0xfe081f00, 0x00080020, 0xfe202081, 0x10004040,
        0xfea49f80, 0xfd00620f, 0xdb0f49c0, 0xdb0fc940,
        0xfea01f80, 0x9000e06f, 0x83f9223e, 0x00000000,
        0xfe0c1f80, 0x00370000, 0xffa01f00, 0x80000040,
        0xff101f00, 0x800c0060, 0x7f0c1f80, 0x80370040,
        0x0000103f, 0x00000000, 0xffa01f00, 0x80000000,
        0xff001f00, 0x800c0020, 0x02c51f01, 0x80000040,
        0xfeac9f80, 0x80000060, 0x0000103f, 0x398ee33f,
        0xfea01f00, 0x80000000, 0x02c19f01, 0x9000e02f,
        0x01c41f01, 0x9000e04f, 0x02c59f80, 0x80000060,
        0x398ee33f, 0x00000000, 0x01c49f01, 0x80000020,
        0x02c11f80, 0x80000040, 0x01e08f00, 0xfe04624f,
        0x01c01f81, 0x7f08626f, 0xfe2c2000, 0x10004000,
        0xfe28a080, 0x10004020, 0xeb825790, 0xb6f711be,
        0x7c0e2df2, 0x81173cfa};

static const uint32_t cpVertexShaderRegs[] = {
        0x00000103, 0x00000000, 0x00000000, 0x00000001,
        0xffffff00, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0x00000000, 0xfffffffc,
        0x00000002, 0x00000000, 0x00000001, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x000000ff, 0x000000ff, 0x000000ff,
        0x000000ff, 0x00000000, 0x0000000e, 0x00000010};

static const uint32_t cPixelShaderProgram[] = {
        0x20000000, 0x00000ca4, 0x0b000000, 0x00000085,
        0x24000000, 0x000050a0, 0xb0000000, 0x000cc080,
        0x39000000, 0x00005ca0, 0xb8000000, 0x000cc080,
        0x51000000, 0x000078a0, 0xc0000000, 0x000cc080,
        0x70000000, 0x000064a0, 0xc8000000, 0x0008c080,
        0x8a000000, 0x00005ca0, 0x0e000000, 0x01008086,
        0xce000000, 0x0000c080, 0xa2000000, 0x00000ca8,
        0x00800000, 0x88062094, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00051f00, 0x80060000, 0x00011f80, 0x80060040,
        0xfec81f80, 0xfb802320, 0x01041f80, 0x8c220000,
        0x00a41f00, 0xfc10620f, 0x010d1f00, 0x900c0021,
        0x00091f00, 0x80060040, 0x00a01f80, 0xfc10626f,
        0x00000040, 0x00000000, 0xfe080000, 0xfe8cc300,
        0xfe088080, 0xfe80c320, 0x00a11f00, 0xfe000200,
        0x00a51f00, 0xfe040220, 0x00a19f00, 0xfe000240,
        0x00a59f00, 0xfe040260, 0x00a11f81, 0xfe002600,
        0x4260e5bc, 0xa69bc4bc, 0x0ad7a3bc, 0x00000000,
        0x00a11f00, 0x06004200, 0x00a59f00, 0x06042220,
        0x00a51f00, 0x06044240, 0x00a11f01, 0x06008260,
        0x00a51f81, 0x06048620, 0x6f1283bc, 0x0ad7a3bc,
        0xa69b44bc, 0x00000000, 0x00a41f00, 0x80000000,
        0x00a01f00, 0x80000020, 0x00ac1f00, 0x80000040,
        0x00a81f00, 0x80000060, 0x00a19f80, 0x06000600,
        0xcac3123c, 0x6f1203bc, 0x03a41f00, 0xfe00620f,
        0x03a01f00, 0xfe04622f, 0x03ac1f00, 0xfe08624f,
        0x03a81f00, 0xfe0c626f, 0x00a59f80, 0x06040620,
        0xcc28913b, 0x6f1203bc, 0x01a41f00, 0xfe00620f,
        0x01a01f00, 0xfe04622f, 0x01ac1f00, 0xfe08624f,
        0x01a81f00, 0xfe0c626f, 0x00a19f80, 0x06002600,
        0xe8eab03c, 0x6f1283bb, 0x02ac1f00, 0xfe084200,
        0x02a81f00, 0xfe0c4220, 0x02a41f00, 0xfe004240,
        0x02a01f00, 0xfe044260, 0x00a59f80, 0x06042620,
        0x92bb353d, 0x6f1283bb, 0x04a81f00, 0x0204620f,
        0x04ac1f00, 0x0200662f, 0x04a41f00, 0x0208624f,
        0x04a01f00, 0x020c626f, 0x00a19f80, 0x06004600,
        0xc4139f3d, 0x6f12833b, 0x00a41f00, 0xfe08620f,
        0x00a01f00, 0xfe0c622f, 0x00ac1f00, 0xfe04624f,
        0x00a81f00, 0xfe00626f, 0x00a59f80, 0x06044620,
        0xb950ed3d, 0x6f12833b, 0x01a41f00, 0xfe00620f,
        0x01a01f00, 0xfe04622f, 0x01ac1f00, 0xfe08624f,
        0x01a81f00, 0xfe0c626f, 0x00a19f80, 0x06002600,
        0xecd7163e, 0x6f12033c, 0x03a41f00, 0xfe000200,
        0x03a01f00, 0xfe040220, 0x03ac1f00, 0xfe082240,
        0x03a81f00, 0xfe0c2260, 0x00a59f80, 0x06042620,
        0x2168233e, 0x6f12033c, 0x00a11f00, 0x06006200,
        0x00a51f00, 0x06046220, 0x00a19f00, 0x06006240,
        0x00a59f00, 0x06046260, 0x00a11f81, 0x0600e600,
        0xa69b443c, 0x6f12833c, 0x0ad7a33c, 0x00000000,
        0x02ac1f00, 0x0108620f, 0x02a81f00, 0x010c622f,
        0x02a41f00, 0x0000624f, 0x02a01f00, 0x0004666f,
        0x00a59f80, 0x0604e620, 0xecd7163e, 0x0ad7a33c,
        0x04a81f00, 0xfe04620f, 0x04ac1f00, 0xfe00622f,
        0x04a41f00, 0xfe08624f, 0x04a01f00, 0xfe0c626f,
        0x00a19f80, 0x06008600, 0xb950ed3d, 0xa69bc43c,
        0x05a41f00, 0xfe08620f, 0x05a01f00, 0xfe0c622f,
        0x05ac1f00, 0xfe04624f, 0x05a81f00, 0xfe00626f,
        0x00a59f80, 0x06048620, 0xc4139f3d, 0xa69bc43c,
        0x03a41f00, 0xfe00a200, 0x03a01f00, 0xfe04a220,
        0x03ac1f00, 0xfe086240, 0x03a81f00, 0xfe0c6260,
        0x00a19f80, 0x06006600, 0x92bb353d, 0x4260e53c,
        0x00a51f80, 0x06046220, 0x4260e53c, 0x00000000,
        0x07ac1f00, 0x0308620f, 0x07a81f00, 0x030c622f,
        0x07a41f00, 0x0500624f, 0x07a01f80, 0x0504626f,
        0xe8eab03c, 0x00000000, 0x04a81f00, 0xfe04620f,
        0x04ac1f00, 0xfe00622f, 0x04a41f00, 0xfe08624f,
        0x04a01f80, 0xfe0c626f, 0xcac3123c, 0x00000000,
        0x06a41f00, 0xfe08620f, 0x06a01f00, 0xfe0c622f,
        0x06ac1f00, 0xfe04624f, 0x06a81f80, 0xfe00626f,
        0xcc28913b, 0x00000000, 0xfe20a000, 0x9000e00f,
        0xfe242000, 0x9000e02f, 0xfe28a001, 0x9000e04f,
        0xfe2c2081, 0x9000e06f, 0xfe28a081, 0x80060020,
        0xfee48f00, 0x7f842300, 0xfee40f00, 0x7f802320,
        0xfee48f01, 0x7f8c2340, 0xfee40f81, 0x08842b60,
        0x00202000, 0x90002000, 0x0024a000, 0x90002020,
        0x00282001, 0x90002040, 0x002ca081, 0x90002060,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x10000000, 0x03100df0, 0x00008010, 0xecdfea0d,
        0x10000000, 0x00100df0, 0x0000a051, 0xecdfea0d,
        0x10000100, 0x01100df0, 0x00008010, 0xecdfea0d,
        0x10000200, 0x02100df0, 0x00000011, 0xecdfea0d,
        0x10000400, 0x04100df0, 0x0000b070, 0xecdfea0d,
        0x10000000, 0x00100df0, 0x00008010, 0xecdfea0d,
        0x10000100, 0x01100df0, 0x00008010, 0xecdfea0d,
        0x10000600, 0x03100df0, 0x00008010, 0xecdfea0d,
        0x10000200, 0x02100df0, 0x00008010, 0xecdfea0d,
        0x10000100, 0x04100df0, 0x00008010, 0xecdfea0d,
        0x10000300, 0x05100df0, 0x00008010, 0xecdfea0d,
        0x10000300, 0x03100df0, 0x0000a051, 0xecdfea0d,
        0x10000700, 0x07100df0, 0x00008010, 0xecdfea0d,
        0x10000400, 0x04100df0, 0x00008010, 0xecdfea0d,
        0x10000300, 0x06100df0, 0x00008010, 0xecdfea0d,
        0x10000000, 0x00100df0, 0x00008010, 0xecdfea0d,
        0xc8581837, 0x22740275, 0x281eddcc, 0xfa8b9b65};
static const uint32_t cPixelShaderRegs[] = {
        0x00000109, 0x00000002, 0x14000001, 0x00000000,
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

Texture2DShader *Texture2DShader::shaderInstance = NULL;

Texture2DShader::Texture2DShader()
    : vertexShader(cuAttributeCount) {
    //! create pixel shader
    pixelShader.setProgram(cPixelShaderProgram, sizeof(cPixelShaderProgram), cPixelShaderRegs, sizeof(cPixelShaderRegs));

    blurLocation           = 0;
    colorIntensityLocation = 4;
    pixelShader.addUniformVar((GX2UniformVar){
            "unf_blur_texture_direction", GX2_SHADER_VAR_TYPE_FLOAT3, 1, blurLocation, -1});
    pixelShader.addUniformVar((GX2UniformVar){
            "unf_color_intensity", GX2_SHADER_VAR_TYPE_FLOAT4, 1, colorIntensityLocation, -1});

    samplerLocation = 0;
    pixelShader.addSamplerVar((GX2SamplerVar){
            "sampl_texture", GX2_SAMPLER_VAR_TYPE_SAMPLER_2D, samplerLocation});

    //! create vertex shader
    vertexShader.setProgram(cpVertexShaderProgram, sizeof(cpVertexShaderProgram), cpVertexShaderRegs, sizeof(cpVertexShaderRegs));

    angleLocation  = 0;
    offsetLocation = 4;
    scaleLocation  = 8;
    vertexShader.addUniformVar((GX2UniformVar){
            "unf_angle", GX2_SHADER_VAR_TYPE_FLOAT, 1, angleLocation, -1});
    vertexShader.addUniformVar((GX2UniformVar){
            "unf_offset", GX2_SHADER_VAR_TYPE_FLOAT3, 1, offsetLocation, -1});
    vertexShader.addUniformVar((GX2UniformVar){
            "unf_scale", GX2_SHADER_VAR_TYPE_FLOAT3, 1, scaleLocation, -1});

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

    //! defaults for normal square
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

Texture2DShader::~Texture2DShader() {
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
