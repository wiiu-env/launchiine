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
#include <video/shaders/Shader3D.h>

static const uint32_t cpVertexShaderProgram[] = {
        0x00000000, 0x00008009, 0x20000000, 0x0000e4a1,
        0x00c00100, 0x88048093, 0x01c00300, 0x98060014,
        0x9a000000, 0x000058a0, 0x3c200200, 0x88062094,
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
        0x0765a101, 0x9000e00f, 0x0761a101, 0x9000e02f,
        0x01081f00, 0x900ce040, 0x01041f00, 0x900ce060,
        0x01001f80, 0x900ce000, 0x02001f00, 0x900c6000,
        0x02041f00, 0x900c6020, 0x076da101, 0x9000e04f,
        0x0769a181, 0x9000e06f, 0x0745a101, 0x9000c00f,
        0x0741a181, 0x9000c02f, 0x074da101, 0x9000c04f,
        0x0749a181, 0x9000c06f, 0x0bc9a000, 0x7f00e20f,
        0x0bc92080, 0x7f04e22f, 0x0bc9a001, 0x7f08e24f,
        0x0bc92081, 0x7f0ce26f, 0x0725a101, 0x9000a00f,
        0x0721a181, 0x9000a02f, 0x072da101, 0x9000a04f,
        0x0729a181, 0x9000a06f, 0x0ac9a000, 0x7e00c20f,
        0x0ac92080, 0x7e04c22f, 0x0ac9a001, 0x7e08c24f,
        0x0ac92081, 0x7e0cc26f, 0x0ba5a000, 0x7f00e20f,
        0x0ba52080, 0x7f04e22f, 0x0ba5a001, 0x7f08e24f,
        0x0ba52081, 0x7f0ce26f, 0x08eda000, 0x9000800f,
        0x08ed2080, 0x9000802f, 0x08eda001, 0x9000804f,
        0x08ed2081, 0x9000806f, 0x09c9a000, 0x7d00a20f,
        0x09c92080, 0x7d04a22f, 0x09c9a001, 0x7d08a24f,
        0x09c92081, 0x7d0ca26f, 0x0aa5a000, 0x7e00c20f,
        0x0aa52080, 0x7e04c22f, 0x0aa5a001, 0x7e08c24f,
        0x0aa52081, 0x7e0cc26f, 0x0b81a000, 0x7f004200,
        0x0b812080, 0x7f044220, 0x0b81a001, 0x7f082240,
        0x0b812081, 0x7f0c0260, 0x08c9a000, 0x7c00820f,
        0x08c92080, 0x7c04822f, 0x08c9a001, 0x7c08824f,
        0x08c92081, 0x7c0c826f, 0x09a5a000, 0x7d00a20f,
        0x09a52080, 0x7d04a22f, 0x09a5a001, 0x7d08a24f,
        0x09a52081, 0x7d0ca26f, 0x0a81a000, 0x7e000200,
        0x0a812080, 0x7e040220, 0x0a81a001, 0x7e080240,
        0x0a812081, 0x7e0c2260, 0x0240a001, 0x9000c00f,
        0x0244a001, 0x9000c02f, 0x0148a001, 0x9000c04f,
        0x004ca001, 0x9000c06f, 0x0264a081, 0x9000e02f,
        0x0260a001, 0x9000e00f, 0x0224a001, 0x90002020,
        0x0168a001, 0x9000e04f, 0x006ca001, 0x9000e06f,
        0x0220a081, 0x90002000, 0x08a5a000, 0x7c00820f,
        0x08a52080, 0x7c04822f, 0x08a5a001, 0x7c08824f,
        0x08a52081, 0x7c0c826f, 0x0981a000, 0x7d008200,
        0x09812080, 0x7d048220, 0x0981a001, 0x7d084240,
        0x09812081, 0x7d0c4260, 0x02090000, 0x7e00c20f,
        0x02098000, 0x7e04c22f, 0x0128a001, 0x9000a04f,
        0x002ca001, 0x9000c06f, 0x02298081, 0x7e0caa6f,
        0x03090000, 0x7f00e20f, 0x03098000, 0x7f04e22f,
        0x02090001, 0x7e08f64f, 0x03298001, 0x7f0ce26f,
        0x03090081, 0x7f08ca4f, 0x0881a000, 0x7c00c200,
        0x08812080, 0x7c04e220, 0x0881a001, 0x7c08a240,
        0x08812081, 0x7c0c8260, 0x0200a001, 0x9000800f,
        0x0204a001, 0x9000802f, 0x0108a001, 0x9000804f,
        0x000ca001, 0x9000806f, 0x01098080, 0x0104aa2f,
        0x01090000, 0x0100a20f, 0x02858000, 0x7e04c22f,
        0x01090001, 0x7d08a24f, 0x01298081, 0x7e0cc26f,
        0x02850000, 0x7e00f60f, 0x03858000, 0x7f04622f,
        0x02450001, 0x7f08e24f, 0x02458001, 0x7d0ca26f,
        0x03850080, 0x7f00ca0f, 0x00090000, 0x7c004200,
        0x00098000, 0x7c04b220, 0x03450001, 0x7e08c24f,
        0x03458001, 0x7f0ce26f, 0x03e18080, 0xfe042620,
        0x01850000, 0x7d00a200, 0x01858000, 0x7d04622f,
        0x00090001, 0x7c086240, 0x00298081, 0x7c0c0260,
        0x02c10000, 0x7f000200, 0x02e18000, 0x7e040620,
        0x01450001, 0x7d088240, 0x01458001, 0x7e0c6260,
        0x01e18080, 0xfe04c620, 0x03c10000, 0x7e002200,
        0x03818001, 0x7f0c4220, 0x02a10001, 0x7f081640,
        0x02818001, 0x7d0c3660, 0x03a10081, 0x7e082a40,
        0x07080000, 0x0100c20f, 0x07088000, 0x0104622f,
        0x00458001, 0x000cea4f, 0x07288081, 0x0204f66f,
        0x00850000, 0x0200620f, 0x00858000, 0x05046a2f,
        0x07080001, 0x0108c24f, 0x01818001, 0x030c726f,
        0x07cc8080, 0xfe04c22f, 0x01c10000, 0x0500660f,
        0x00e18000, 0xfe04622f, 0x00450001, 0x0308624f,
        0x07cc9f01, 0x7f0ce26f, 0x00c10080, 0xfe00e60f,
        0x07cc1f00, 0x7e00660f, 0x00a10001, 0xfe08c22f,
        0x01a10001, 0x0408624f, 0x00818001, 0x7f086a6f,
        0x07c09f80, 0x7e048200, 0x07e00f00, 0xfe008220,
        0x07cc1f01, 0x7e086a4f, 0x07c09f81, 0x7f0c8240,
        0x07c08f80, 0xfe088260, 0x2c34800d, 0xe3b4f15e,
        0x7642ed30, 0x7408600d};

static const uint32_t cpVertexShaderRegs[] = {
        0x00000108, 0x00000000, 0x00000002, 0x00000001,
        0xffff0001, 0xffffffff, 0xffffffff, 0xffffffff,
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
        0x20000000, 0x000008a4, 0x03000000, 0x01004085,
        0x23000000, 0x000044a8, 0x35000000, 0x000000a4,
        0x06000000, 0x01004085, 0x36000000, 0x00002ca8,
        0x50000000, 0x0000c080, 0x42000000, 0x00001ca0,
        0x00800000, 0x88062094, 0x00000000, 0x00000000,
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
        0xfd001f80, 0x900c0060, 0x0000803f, 0x00000000,
        0x02011f80, 0x8c110000, 0xf8402000, 0x9006a00f,
        0x02552001, 0x00000020, 0x01248082, 0x80020060,
        0xfe3c1f00, 0x1000e04f, 0xfe041f80, 0x1033c00f,
        0xfe482081, 0x80060020, 0xfee40f81, 0x0289e30f,
        0x02c51f80, 0x80060060, 0xfeec0f80, 0x0285634f,
        0xfec80f80, 0x80000060, 0xfe4ca081, 0x9000e04f,
        0xfe281f00, 0x80060000, 0xf8c01f81, 0x9006e02f,
        0xfee00f81, 0xfd80636f, 0x0000803f, 0x00000000,
        0x7fc49f81, 0xf880e34f, 0xfe381f80, 0x00000000,
        0x7de00f81, 0xfe800360, 0x01011f80, 0x8c100000,
        0x00a81f00, 0x9000e02f, 0x00000082, 0x80020060,
        0x00002040, 0x00000000, 0xfeac9f80, 0xfd00624f,
        0x3333333f, 0x00002040, 0xfee88f80, 0x0101620f,
        0x00cc1f80, 0x9000e06f, 0xf8c09f01, 0x80060020,
        0xfe2c1f80, 0x9006e04f, 0xfee48f81, 0xf880630f,
        0x7fc81f80, 0xfd800360, 0x0000803f, 0x00000000,
        0x000ca001, 0x80000000, 0x00091f00, 0x800c0020,
        0x00051f00, 0x800c0040, 0x00011f80, 0x800c0060,
        0xfe2c0000, 0x90002000, 0xfe288000, 0x90002020,
        0xfe240001, 0x90002040, 0xfe208081, 0x90002060,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x10000100, 0x01100df0, 0x00008010, 0xecdfea0d,
        0x99720984, 0x041cab0d, 0xa28a9ccd, 0x95d199a5};
static const uint32_t cPixelShaderRegs[] = {
        0x00000102, 0x00000002, 0x14000002, 0x00000000,
        0x00000002, 0x00000100, 0x00000101, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x0000000f, 0x00000001, 0x00000010,
        0x00000000};

Shader3D *Shader3D::shaderInstance = NULL;

Shader3D::Shader3D()
    : vertexShader(cuAttributeCount) {
    //! create pixel shader
    pixelShader.setProgram(cPixelShaderProgram, sizeof(cPixelShaderProgram), cPixelShaderRegs, sizeof(cPixelShaderRegs));

    colorIntensityLocation = 0;
    fadeDistanceLocation   = 4;
    fadeOutLocation        = 8;
    pixelShader.addUniformVar((GX2UniformVar){
            "unf_color_intensity", GX2_SHADER_VAR_TYPE_FLOAT4, 1, colorIntensityLocation, -1});
    pixelShader.addUniformVar((GX2UniformVar){
            "unf_fade_distance", GX2_SHADER_VAR_TYPE_FLOAT, 1, fadeDistanceLocation, -1});
    pixelShader.addUniformVar((GX2UniformVar){
            "unf_fade_out_alpha", GX2_SHADER_VAR_TYPE_FLOAT4, 1, fadeOutLocation, -1});

    samplerLocation = 0;
    pixelShader.addSamplerVar((GX2SamplerVar){
            "sampl_texture", GX2_SAMPLER_VAR_TYPE_SAMPLER_2D, samplerLocation});
    //! create vertex shader
    vertexShader.setProgram(cpVertexShaderProgram, sizeof(cpVertexShaderProgram), cpVertexShaderRegs, sizeof(cpVertexShaderRegs));

    modelMatrixLocation      = 0;
    projectionMatrixLocation = 16;
    viewMatrixLocation       = 32;
    vertexShader.addUniformVar((GX2UniformVar){
            "modelMatrix", GX2_SHADER_VAR_TYPE_FLOAT4X4, 1, modelMatrixLocation, -1});
    vertexShader.addUniformVar((GX2UniformVar){
            "viewMatrix", GX2_SHADER_VAR_TYPE_FLOAT4X4, 1, projectionMatrixLocation, -1});
    vertexShader.addUniformVar((GX2UniformVar){
            "projectionMatrix", GX2_SHADER_VAR_TYPE_FLOAT4X4, 1, viewMatrixLocation, -1});

    positionLocation = 0;
    texCoordLocation = 1;
    vertexShader.addAttribVar((GX2AttribVar){
            "attr_position", GX2_SHADER_VAR_TYPE_FLOAT4, 0, positionLocation});
    vertexShader.addAttribVar((GX2AttribVar){
            "attr_texture_coord", GX2_SHADER_VAR_TYPE_FLOAT2, 0, texCoordLocation});

    //! setup attribute streams
    GX2InitAttribStream(vertexShader.getAttributeBuffer(0), positionLocation, 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32);
    GX2InitAttribStream(vertexShader.getAttributeBuffer(1), texCoordLocation, 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);

    //! create fetch shader
    fetchShader = new FetchShader(vertexShader.getAttributeBuffer(), vertexShader.getAttributesCount());

    //! initialize default quad texture vertexes as those are very commonly used
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

Shader3D::~Shader3D() {
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
