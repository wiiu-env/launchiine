#ifndef _GAME_ICON_H_
#define _GAME_ICON_H_

#include <gui/GuiImageAsync.h>
#include <video/shaders/Shader3D.h>

class GameIcon : public GuiImage {
public:
    GameIcon(GuiImageData *preloadImage);

    virtual ~GameIcon();

    void setRotationX(float r) {
        rotationX = r;
    }

    void setColorIntensity(const glm::vec4 &color) {
        colorIntensity                   = color;
        colorIntensityMirror             = colorIntensity;
        selectionBlurOuterColorIntensity = color * glm::vec4(0.09411764f * 1.15f, 0.56862745f * 1.15f, 0.96862745098f * 1.15f, 1.0f);
        selectionBlurInnerColorIntensity = color * glm::vec4(0.46666667f, 0.90588235f, 1.0f, 1.0f);
    }

    const glm::vec4 &getColorIntensity() const {
        return colorIntensity;
    }

    void setAlphaFadeOutNorm(const glm::vec4 &a) {
        alphaFadeOutNorm = a;
    }

    void setAlphaFadeOutRefl(const glm::vec4 &a) {
        alphaFadeOutRefl = a;
    }

    void setRenderReflection(bool enable) {
        bRenderReflection = enable;
    }

    void setSelected(bool enable) {
        bSelected = enable;
    }

    void setStrokeRender(bool enable) {
        bRenderStroke = enable;
    }

    void setRenderIconLast(bool enable) {
        bIconLast = enable;
    }

    void draw(CVideo *pVideo) {
        static const glm::mat4 identity(1.0f);
        draw(pVideo, identity, identity, identity);
    }

    void draw(CVideo *pVideo, const glm::mat4 &projection, const glm::mat4 &view, const glm::mat4 &modelView);

    bool checkRayIntersection(const glm::vec3 &rayOrigin, const glm::vec3 &rayDirFrac);

private:
    enum eRenderState {
        RENDER_REFLECTION,
        RENDER_NORMAL
    };

    bool bSelected;
    bool bRenderStroke;
    bool bRenderReflection;
    bool bIconLast;
    glm::vec4 colorIntensity;
    glm::vec4 colorIntensityMirror;
    glm::vec4 alphaFadeOutNorm;
    glm::vec4 alphaFadeOutRefl;

    float reflectionAlpha;
    float strokeWidth;
    float rotationX;
    float rgbReduction;
    float distanceFadeout;
    float *texCoordsMirror;
    float *strokePosVtxs;
    float *strokeTexCoords;
    uint8_t *strokeColorVtxs;
    int32_t strokeFractalEnable;
    float strokeBlurBorder;
    glm::vec4 selectionBlurOuterColorIntensity;
    float selectionBlurOuterSize;
    float selectionBlurOuterBorderSize;
    glm::vec4 selectionBlurInnerColorIntensity;
    float selectionBlurInnerSize;
    float selectionBlurInnerBorderSize;
};

#endif // _GAME_ICON_H_
