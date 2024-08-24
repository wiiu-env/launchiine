#ifndef __GX2_EXTENSION_H
#define __GX2_EXTENSION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <gx2/draw.h>
#include <gx2/enum.h>
#include <gx2/mem.h>
#include <gx2/registers.h>
#include <gx2/sampler.h>
#include <gx2/shaders.h>
#include <gx2/surface.h>
#include <gx2/texture.h>

#define GX2_AA_BUFFER_CLEAR_VALUE 0xCC

#define GX2_COMP_SEL_NONE         0x04040405
#define GX2_COMP_SEL_X001         0x00040405
#define GX2_COMP_SEL_XY01         0x00010405
#define GX2_COMP_SEL_XYZ1         0x00010205
#define GX2_COMP_SEL_XYZW         0x00010203
#define GX2_COMP_SEL_XXXX         0x00000000
#define GX2_COMP_SEL_YYYY         0x01010101
#define GX2_COMP_SEL_ZZZZ         0x02020202
#define GX2_COMP_SEL_WWWW         0x03030303
#define GX2_COMP_SEL_WZYX         0x03020100
#define GX2_COMP_SEL_WXYZ         0x03000102

typedef struct _GX2Color {
    uint8_t r, g, b, a;
} GX2Color;

typedef struct _GX2ColorF32 {
    float r, g, b, a;
} GX2ColorF32;

static const uint32_t attribute_dest_comp_selector[20] = {
        GX2_COMP_SEL_X001, GX2_COMP_SEL_XY01, GX2_COMP_SEL_X001, GX2_COMP_SEL_X001, GX2_COMP_SEL_XY01, GX2_COMP_SEL_X001,
        GX2_COMP_SEL_X001, GX2_COMP_SEL_XY01, GX2_COMP_SEL_XY01, GX2_COMP_SEL_XYZ1, GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZW,
        GX2_COMP_SEL_XY01, GX2_COMP_SEL_XY01, GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZ1, GX2_COMP_SEL_XYZ1,
        GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZW};

static const uint32_t texture_comp_selector[54] = {
        GX2_COMP_SEL_NONE, GX2_COMP_SEL_X001, GX2_COMP_SEL_XY01, GX2_COMP_SEL_NONE, GX2_COMP_SEL_NONE, GX2_COMP_SEL_X001,
        GX2_COMP_SEL_X001, GX2_COMP_SEL_XY01, GX2_COMP_SEL_XYZ1, GX2_COMP_SEL_XYZ1, GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZW,
        GX2_COMP_SEL_WZYX, GX2_COMP_SEL_X001, GX2_COMP_SEL_X001, GX2_COMP_SEL_XY01, GX2_COMP_SEL_XY01, GX2_COMP_SEL_NONE,
        GX2_COMP_SEL_NONE, GX2_COMP_SEL_NONE, GX2_COMP_SEL_NONE, GX2_COMP_SEL_NONE, GX2_COMP_SEL_XYZ1, GX2_COMP_SEL_NONE,
        GX2_COMP_SEL_NONE, GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZW, GX2_COMP_SEL_WZYX, GX2_COMP_SEL_XY01, GX2_COMP_SEL_XY01,
        GX2_COMP_SEL_XY01, GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZW, GX2_COMP_SEL_NONE, GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZW,
        GX2_COMP_SEL_NONE, GX2_COMP_SEL_NONE, GX2_COMP_SEL_NONE, GX2_COMP_SEL_XYZ1, GX2_COMP_SEL_XYZ1, GX2_COMP_SEL_X001,
        GX2_COMP_SEL_XY01, GX2_COMP_SEL_XYZ1, GX2_COMP_SEL_NONE, GX2_COMP_SEL_NONE, GX2_COMP_SEL_NONE, GX2_COMP_SEL_XYZ1,
        GX2_COMP_SEL_XYZ1, GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZW, GX2_COMP_SEL_XYZW, GX2_COMP_SEL_X001, GX2_COMP_SEL_XY01};

static inline void GX2InitDepthBuffer(GX2DepthBuffer *depthBuffer, GX2SurfaceDim dim, uint32_t width, uint32_t height, uint32_t depth, GX2SurfaceFormat format, GX2AAMode aa) {
    depthBuffer->surface.dim       = dim;
    depthBuffer->surface.width     = width;
    depthBuffer->surface.height    = height;
    depthBuffer->surface.depth     = depth;
    depthBuffer->surface.mipLevels = 1;
    depthBuffer->surface.format    = format;
    depthBuffer->surface.aa        = aa;
    depthBuffer->surface.use       = (GX2SurfaceUse) (((format == GX2_SURFACE_FORMAT_UNORM_R24_X8) || (format == GX2_SURFACE_FORMAT_FLOAT_D24_S8)) ? GX2_SURFACE_USE_DEPTH_BUFFER : (GX2_SURFACE_USE_DEPTH_BUFFER | GX2_SURFACE_USE_TEXTURE));
    depthBuffer->surface.tileMode  = GX2_TILE_MODE_DEFAULT;
    depthBuffer->surface.swizzle   = 0;
    depthBuffer->viewMip           = 0;
    depthBuffer->viewFirstSlice    = 0;
    depthBuffer->viewNumSlices     = depth;
    depthBuffer->depthClear        = 1.0f;
    depthBuffer->stencilClear      = 0;
    depthBuffer->hiZPtr            = NULL;
    depthBuffer->hiZSize           = 0;
    GX2CalcSurfaceSizeAndAlignment(&depthBuffer->surface);
    GX2InitDepthBufferRegs(depthBuffer);
}

static inline void GX2InitColorBuffer(GX2ColorBuffer *colorBuffer, GX2SurfaceDim dim, uint32_t width, uint32_t height, uint32_t depth, GX2SurfaceFormat format, GX2AAMode aa) {
    colorBuffer->surface.dim        = dim;
    colorBuffer->surface.width      = width;
    colorBuffer->surface.height     = height;
    colorBuffer->surface.depth      = depth;
    colorBuffer->surface.mipLevels  = 1;
    colorBuffer->surface.format     = format;
    colorBuffer->surface.aa         = aa;
    colorBuffer->surface.use        = GX2_SURFACE_USE_TEXTURE_COLOR_BUFFER_TV;
    colorBuffer->surface.imageSize  = 0;
    colorBuffer->surface.image      = NULL;
    colorBuffer->surface.mipmapSize = 0;
    colorBuffer->surface.mipmaps    = NULL;
    colorBuffer->surface.tileMode   = GX2_TILE_MODE_DEFAULT;
    colorBuffer->surface.swizzle    = 0;
    colorBuffer->surface.alignment  = 0;
    colorBuffer->surface.pitch      = 0;
    uint32_t i;
    for (i = 0; i < 13; i++) {
        colorBuffer->surface.mipLevelOffset[i] = 0;
    }
    colorBuffer->viewMip        = 0;
    colorBuffer->viewFirstSlice = 0;
    colorBuffer->viewNumSlices  = depth;
    colorBuffer->aaBuffer       = NULL;
    colorBuffer->aaSize         = 0;
    for (i = 0; i < 5; i++) {
        colorBuffer->regs[i] = 0;
    }

    GX2CalcSurfaceSizeAndAlignment(&colorBuffer->surface);
    GX2InitColorBufferRegs(colorBuffer);
}

static inline void GX2InitAttribStream(GX2AttribStream *attr, uint32_t location, uint32_t buffer, uint32_t offset, GX2AttribFormat format) {
    attr->location   = location;
    attr->buffer     = buffer;
    attr->offset     = offset;
    attr->format     = format;
    attr->type       = GX2_ATTRIB_INDEX_PER_VERTEX;
    attr->aluDivisor = 0;
    attr->mask       = attribute_dest_comp_selector[format & 0xff];
    attr->endianSwap = GX2_ENDIAN_SWAP_DEFAULT;
}

static inline void GX2InitTexture(GX2Texture *tex, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevels, GX2SurfaceFormat format, GX2SurfaceDim dim, GX2TileMode tile) {
    tex->surface.dim        = dim;
    tex->surface.width      = width;
    tex->surface.height     = height;
    tex->surface.depth      = depth;
    tex->surface.mipLevels  = mipLevels;
    tex->surface.format     = format;
    tex->surface.aa         = GX2_AA_MODE1X;
    tex->surface.use        = GX2_SURFACE_USE_TEXTURE;
    tex->surface.imageSize  = 0;
    tex->surface.image      = NULL;
    tex->surface.mipmapSize = 0;
    tex->surface.mipmaps    = NULL;
    tex->surface.tileMode   = tile;
    tex->surface.swizzle    = 0;
    tex->surface.alignment  = 0;
    tex->surface.pitch      = 0;
    uint32_t i;
    for (i = 0; i < 13; i++) {
        tex->surface.mipLevelOffset[i] = 0;
    }
    tex->viewFirstMip   = 0;
    tex->viewNumMips    = mipLevels;
    tex->viewFirstSlice = 0;
    tex->viewNumSlices  = depth;
    tex->compMap        = texture_comp_selector[format & 0x3f];
    for (i = 0; i < 5; i++) {
        tex->regs[i] = 0;
    }

    GX2CalcSurfaceSizeAndAlignment(&tex->surface);
    GX2InitTextureRegs(tex);
}

#ifdef __cplusplus
}
#endif

#endif
