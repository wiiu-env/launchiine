#include "GameIcon.h"
#include "GameIconModel.h"
#include "Application.h"
#include <video/CVideo.h>
#include <video/shaders/Shader3D.h>
#include <video/shaders/ShaderFractalColor.h>
#include "utils/utils.h"
#include "utils/logger.h"

static const float cfIconMirrorScale = 1.15f;
static const float cfIconMirrorAlpha = 0.45f;

GameIcon::GameIcon(GuiImageData *preloadImage)
        : GuiImage(preloadImage) {
    bSelected = false;
    bRenderStroke = true;
    bRenderReflection = false;
    bIconLast = false;
    strokeFractalEnable = 1;
    strokeBlurBorder = 0.0f;
    distanceFadeout = 0.0f;
    rotationX = 0.0f;
    reflectionAlpha = 0.4f;
    strokeWidth = 2.35f;
    colorIntensity = glm::vec4(1.0f);
    colorIntensityMirror = colorIntensity;
    alphaFadeOutNorm = glm::vec4(0.0f);
    alphaFadeOutRefl = glm::vec4(-1.0f, 0.0f, 0.9f, 1.0f);
    selectionBlurOuterColorIntensity = glm::vec4(0.09411764f * 1.15f, 0.56862745f * 1.15f, 0.96862745098f * 1.15f, 1.0f);
    selectionBlurOuterSize = 1.65f;
    selectionBlurOuterBorderSize = 0.5f;
    selectionBlurInnerColorIntensity = glm::vec4(0.46666667f, 0.90588235f, 1.0f, 1.0f);
    selectionBlurInnerSize = 1.45f;
    selectionBlurInnerBorderSize = 0.95f;

    vtxCount = sizeof(cfGameIconPosVtxs) / (Shader3D::cuVertexAttrSize);

    //! texture and vertex coordinates
    posVtxs = (float *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, sizeof(cfGameIconPosVtxs));
    texCoords = (float *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, sizeof(cfGameIconTexCoords));

    if (posVtxs) {
        memcpy((float *) posVtxs, cfGameIconPosVtxs, sizeof(cfGameIconPosVtxs));
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, (float *) posVtxs, sizeof(cfGameIconPosVtxs));
    }
    if (texCoords) {
        memcpy((float *) texCoords, cfGameIconTexCoords, sizeof(cfGameIconTexCoords));
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, (float *) texCoords, sizeof(cfGameIconTexCoords));
    }

    //! create vertexes for the mirror frame
    texCoordsMirror = (float *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, sizeof(cfGameIconTexCoords));

    if (texCoordsMirror) {
        for (uint32_t i = 0; i < vtxCount; i++) {
            texCoordsMirror[i * 2 + 0] = texCoords[i * 2 + 0] * cfIconMirrorScale - ((cfIconMirrorScale - 1.0f) - (cfIconMirrorScale - 1.0f) * 0.5f);
            texCoordsMirror[i * 2 + 1] = texCoords[i * 2 + 1] * cfIconMirrorScale - ((cfIconMirrorScale - 1.0f) - (cfIconMirrorScale - 1.0f) * 0.5f);
        }
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, texCoordsMirror, sizeof(cfGameIconTexCoords));
    }

    //! setup stroke of the icon
    strokePosVtxs = (float *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, sizeof(cfGameIconStrokeVtxs));
    if (strokePosVtxs) {
        memcpy(strokePosVtxs, cfGameIconStrokeVtxs, sizeof(cfGameIconStrokeVtxs));
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, strokePosVtxs, sizeof(cfGameIconStrokeVtxs));
    }
    strokeTexCoords = (float *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, cuGameIconStrokeVtxCount * Shader::cuTexCoordAttrSize);
    if (strokeTexCoords) {
        for (size_t i = 0, n = 0; i < cuGameIconStrokeVtxCount; n += 2, i += 3) {
            strokeTexCoords[n] = (1.0f + strokePosVtxs[i]) * 0.5f;
            strokeTexCoords[n + 1] = 1.0f - (1.0f + strokePosVtxs[i + 1]) * 0.5f;
        }
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, strokeTexCoords, cuGameIconStrokeVtxCount * Shader::cuTexCoordAttrSize);
    }
    strokeColorVtxs = (uint8_t *) memalign(GX2_VERTEX_BUFFER_ALIGNMENT, cuGameIconStrokeVtxCount * Shader::cuColorAttrSize);
    if (strokeColorVtxs) {
        for (size_t i = 0; i < (cuGameIconStrokeVtxCount * Shader::cuColorAttrSize); i++)
            strokeColorVtxs[i] = 0xff;
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, strokeColorVtxs, cuGameIconStrokeVtxCount * Shader::cuColorAttrSize);
    }
}

GameIcon::~GameIcon() {
    //! remove image so it can not be drawn anymore from this point on
    imageData = nullptr;

    //! main image vertexes
    if (posVtxs) {
        free((void *) posVtxs);
        posVtxs = nullptr;
    }
    if (texCoords) {
        free((void *) texCoords);
        texCoords = nullptr;
    }
    //! mirror image vertexes
    if (texCoordsMirror) {
        free(texCoordsMirror);
        texCoordsMirror = nullptr;
    }
    //! stroke image vertexes
    if (strokePosVtxs) {
        free(strokePosVtxs);
        strokePosVtxs = nullptr;
    }
    if (strokeTexCoords) {
        free(strokeTexCoords);
        strokeTexCoords = nullptr;
    }
    if (strokeColorVtxs) {
        free(strokeColorVtxs);
        strokeColorVtxs = nullptr;
    }
}

bool GameIcon::checkRayIntersection(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirFrac) {
    //! since we always face the camera we can just check the AABB intersection
    //! otherwise an OOB intersection would be required

    float currPosX = getCenterX() * Application::instance()->getVideo()->getWidthScaleFactor() * 2.0f;
    float currPosY = getCenterY() * Application::instance()->getVideo()->getHeightScaleFactor() * 2.0f;
    float currPosZ = getDepth() * Application::instance()->getVideo()->getDepthScaleFactor() * 2.0f;
    float currScaleX = getScaleX() * (float) getWidth() * Application::instance()->getVideo()->getWidthScaleFactor();
    float currScaleY = getScaleY() * (float) getHeight() * Application::instance()->getVideo()->getHeightScaleFactor();
    float currScaleZ = getScaleZ() * (float) getWidth() * Application::instance()->getVideo()->getDepthScaleFactor();
    //! lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    glm::vec3 lb(currPosX - currScaleX, currPosY - currScaleY, currPosZ - currScaleZ);
    glm::vec3 rt(currPosX + currScaleX, currPosY + currScaleY, currPosZ + currScaleZ);

    float t1 = (lb.x - rayOrigin.x) * rayDirFrac.x;
    float t2 = (rt.x - rayOrigin.x) * rayDirFrac.x;
    float t3 = (lb.y - rayOrigin.y) * rayDirFrac.y;
    float t4 = (rt.y - rayOrigin.y) * rayDirFrac.y;
    float t5 = (lb.z - rayOrigin.z) * rayDirFrac.z;
    float t6 = (rt.z - rayOrigin.z) * rayDirFrac.z;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    //! if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
    if (tmax < 0) {
        //t = tmax;
        return false;
    }

    //! if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax) {
        //t = tmax;
        return false;
    }

    //t = tmin;
    return true;
}

void GameIcon::draw(CVideo *pVideo, const glm::mat4 &projectionMtx, const glm::mat4 &viewMtx, const glm::mat4 &modelView) {
    if (imageData == nullptr) {
        return;
    }
    //! first setup 2D GUI positions
    float currPosX = getCenterX() * pVideo->getWidthScaleFactor() * 2.0f;
    float currPosY = getCenterY() * pVideo->getHeightScaleFactor() * 2.0f;
    float currPosZ = getDepth() * pVideo->getDepthScaleFactor() * 2.0f;
    float currScaleX = getScaleX() * (float) getWidth() * pVideo->getWidthScaleFactor();
    float currScaleY = getScaleY() * (float) getHeight() * pVideo->getHeightScaleFactor();
    float currScaleZ = getScaleZ() * (float) getWidth() * pVideo->getDepthScaleFactor();
    float strokeScaleX = pVideo->getWidthScaleFactor() * strokeWidth * 0.25f + cfIconMirrorScale;
    float strokeScaleY = pVideo->getHeightScaleFactor() * strokeWidth * 0.25f + cfIconMirrorScale;

    for (int32_t iDraw = 0; iDraw < 2; iDraw++) {
        glm::vec4 *alphaFadeOut;
        glm::mat4 m_iconView;
        glm::mat4 m_mirrorView;
        glm::mat4 m_strokeView;

        if (iDraw == RENDER_REFLECTION) {
            //! Reflection render
            if (!bRenderReflection)
                continue;
            m_iconView = glm::translate(modelView, glm::vec3(currPosX, -currScaleY * 2.0f - currPosY, currPosZ + cosf(DegToRad(rotationX)) * currScaleZ * 2.0f));
            m_iconView = glm::rotate(m_iconView, DegToRad(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
            m_iconView = glm::scale(m_iconView, glm::vec3(currScaleX, -currScaleY, currScaleZ));

            colorIntensity[3] = reflectionAlpha * getAlpha();
            selectionBlurOuterColorIntensity[3] = colorIntensity[3] * 0.7f;
            selectionBlurInnerColorIntensity[3] = colorIntensity[3] * 0.7f;
            alphaFadeOut = &alphaFadeOutRefl;

            GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_ENABLE, GX2_DISABLE);
        } else {
            //! Normal render
            m_iconView = glm::translate(modelView, glm::vec3(currPosX, currPosY, currPosZ));
            m_iconView = glm::rotate(m_iconView, DegToRad(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
            m_iconView = glm::scale(m_iconView, glm::vec3(currScaleX, currScaleY, currScaleZ));

            colorIntensity[3] = getAlpha();
            selectionBlurOuterColorIntensity[3] = colorIntensity[3];
            selectionBlurInnerColorIntensity[3] = colorIntensity[3];
            alphaFadeOut = &alphaFadeOutNorm;
        }

        m_mirrorView = glm::scale(m_iconView, glm::vec3(cfIconMirrorScale, cfIconMirrorScale, cfIconMirrorScale));

        colorIntensityMirror[3] = cfIconMirrorAlpha * colorIntensity[3];

        if (!bIconLast) {
            Shader3D::instance()->setShaders();
            Shader3D::instance()->setProjectionMtx(projectionMtx);
            Shader3D::instance()->setViewMtx(viewMtx);
            Shader3D::instance()->setTextureAndSampler(imageData->getTexture(), imageData->getSampler());
            Shader3D::instance()->setAlphaFadeOut(*alphaFadeOut);
            Shader3D::instance()->setDistanceFadeOut(distanceFadeout);

            //! render the real symbol
            Shader3D::instance()->setModelViewMtx(m_iconView);
            Shader3D::instance()->setColorIntensity(colorIntensity);
            Shader3D::instance()->setAttributeBuffer(vtxCount, posVtxs, texCoords);
            Shader3D::instance()->draw(GX2_PRIMITIVE_MODE_QUADS, vtxCount);
        }


        if (bSelected) {
            strokeFractalEnable = 0;

            GX2SetDepthOnlyControl(GX2_ENABLE, GX2_DISABLE, GX2_COMPARE_FUNC_LEQUAL);
            m_strokeView = glm::scale(m_iconView, glm::vec3(selectionBlurOuterSize, selectionBlurOuterSize, 0.0f));
            ShaderFractalColor::instance()->setShaders();
            ShaderFractalColor::instance()->setProjectionMtx(projectionMtx);
            ShaderFractalColor::instance()->setViewMtx(viewMtx);
            ShaderFractalColor::instance()->setModelViewMtx(m_strokeView);
            ShaderFractalColor::instance()->setFractalColor(strokeFractalEnable);
            ShaderFractalColor::instance()->setBlurBorder(selectionBlurOuterBorderSize);
            ShaderFractalColor::instance()->setColorIntensity(selectionBlurOuterColorIntensity);
            ShaderFractalColor::instance()->setAlphaFadeOut(*alphaFadeOut);
            ShaderFractalColor::instance()->setAttributeBuffer();
            ShaderFractalColor::instance()->draw();

            m_strokeView = glm::scale(m_iconView, glm::vec3(selectionBlurInnerSize, selectionBlurInnerSize, 0.0f));
            ShaderFractalColor::instance()->setBlurBorder(selectionBlurInnerBorderSize);
            ShaderFractalColor::instance()->setColorIntensity(selectionBlurInnerColorIntensity);
            ShaderFractalColor::instance()->draw();
            GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LEQUAL);
        }

        if (iDraw == RENDER_NORMAL && bRenderStroke) {
            strokeFractalEnable = 1;
            //! now render the icon stroke
            //! make the stroke a little bigger than the mirror, just by the line width on each side
            m_strokeView = glm::scale(m_iconView, glm::vec3(strokeScaleX, strokeScaleY, cfIconMirrorScale));

            ShaderFractalColor::instance()->setShaders();
            ShaderFractalColor::instance()->setLineWidth(strokeWidth);
            ShaderFractalColor::instance()->setProjectionMtx(projectionMtx);
            ShaderFractalColor::instance()->setViewMtx(viewMtx);
            ShaderFractalColor::instance()->setModelViewMtx(m_strokeView);
            ShaderFractalColor::instance()->setFractalColor(strokeFractalEnable);
            ShaderFractalColor::instance()->setBlurBorder(strokeBlurBorder);
            ShaderFractalColor::instance()->setColorIntensity(colorIntensity);
            ShaderFractalColor::instance()->setAlphaFadeOut(*alphaFadeOut);
            ShaderFractalColor::instance()->setAttributeBuffer(cuGameIconStrokeVtxCount, strokePosVtxs, strokeTexCoords, strokeColorVtxs);
            ShaderFractalColor::instance()->draw(GX2_PRIMITIVE_MODE_LINE_STRIP, cuGameIconStrokeVtxCount);
        }

        //! render the background mirror frame
        Shader3D::instance()->setShaders();
        Shader3D::instance()->setProjectionMtx(projectionMtx);
        Shader3D::instance()->setViewMtx(viewMtx);
        Shader3D::instance()->setTextureAndSampler(imageData->getTexture(), imageData->getSampler());
        Shader3D::instance()->setAlphaFadeOut(*alphaFadeOut);
        Shader3D::instance()->setDistanceFadeOut(distanceFadeout);
        Shader3D::instance()->setModelViewMtx(m_mirrorView);
        Shader3D::instance()->setColorIntensity(colorIntensityMirror);
        Shader3D::instance()->setAttributeBuffer(vtxCount, posVtxs, texCoordsMirror);
        Shader3D::instance()->draw(GX2_PRIMITIVE_MODE_QUADS, vtxCount);

        if (bIconLast) {
            Shader3D::instance()->setShaders();
            Shader3D::instance()->setProjectionMtx(projectionMtx);
            Shader3D::instance()->setViewMtx(viewMtx);
            Shader3D::instance()->setTextureAndSampler(imageData->getTexture(), imageData->getSampler());
            Shader3D::instance()->setAlphaFadeOut(*alphaFadeOut);
            Shader3D::instance()->setDistanceFadeOut(distanceFadeout);

            //! render the real symbol
            Shader3D::instance()->setModelViewMtx(m_iconView);
            Shader3D::instance()->setColorIntensity(colorIntensity);
            Shader3D::instance()->setAttributeBuffer(vtxCount, posVtxs, texCoords);
            Shader3D::instance()->draw(GX2_PRIMITIVE_MODE_QUADS, vtxCount);
        }

        //! return back normal culling
        if (iDraw == RENDER_REFLECTION) {
            GX2SetCullOnlyControl(GX2_FRONT_FACE_CCW, GX2_DISABLE, GX2_ENABLE);
        }
    }
}
