#include <gui/GameBgImage.h>
#include <video/CVideo.h>
#include <video/shaders/Shader3D.h>

GameBgImage::GameBgImage(const std::string &filename, GuiImageData *preloadImage)
    : GuiImageAsync(filename, preloadImage) {
    identity     = glm::mat4(1.0f);
    alphaFadeOut = glm::vec4(1.0f, 0.075f, 5.305f, 2.0f);
}

GameBgImage::~GameBgImage() {
}

void GameBgImage::draw(CVideo *pVideo) {
    if (!getImageData() || !getImageData()->getTexture()) {
        return;
    }

    //! first setup 2D GUI positions
    float currPosX   = getCenterX();
    float currPosY   = getCenterY();
    float currPosZ   = getDepth();
    float currScaleX = getScaleX() * (float) getWidth() * pVideo->getWidthScaleFactor();
    float currScaleY = getScaleY() * (float) getHeight() * pVideo->getHeightScaleFactor();
    float currScaleZ = getScaleZ() * (float) getWidth() * pVideo->getDepthScaleFactor();

    glm::mat4 m_modelView = glm::translate(identity, glm::vec3(currPosX, currPosY, currPosZ));
    m_modelView           = glm::scale(m_modelView, glm::vec3(currScaleX, currScaleY, currScaleZ));

    Shader3D::instance()->setShaders();
    Shader3D::instance()->setProjectionMtx(identity);
    Shader3D::instance()->setViewMtx(identity);
    Shader3D::instance()->setModelViewMtx(m_modelView);
    Shader3D::instance()->setTextureAndSampler(getImageData()->getTexture(), getImageData()->getSampler());
    Shader3D::instance()->setAlphaFadeOut(alphaFadeOut);
    Shader3D::instance()->setDistanceFadeOut(0.0f);
    Shader3D::instance()->setColorIntensity(glm::vec4(1.0f, 1.0f, 1.0f, getAlpha()));
    Shader3D::instance()->setAttributeBuffer();
    Shader3D::instance()->draw();
}
