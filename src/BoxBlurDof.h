#ifndef BOXBLUR_DOF_H
#define BOXBLUR_DOF_H

#include <raylib.h>
#include <array>
#include "utils.h"
#include "lights.h"

#define GLSL_VERSION 330
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

typedef enum{
    SHADER_BLUR = 0,
    SHADER_DILATION,
    SHADER_DOF
} SHADERS_ENUM;

typedef enum{
    BLUR_TEX = 0,
    DILATION_TEX
} TEXTURES_ENUM;

class BoxBlurDof{
    
    private:
        int boxBlurParamsLoc,boxBlurScreenTexLoc;
        int dilationScreenTexLoc, dilationParamsLoc;
        int blurRadLoc,cocTexLoc,blurredTexLoc;
    
    public:
        std::array<Shader,4> shaders;
        std::array<RenderTexture2D,3> textures;
        // float maxBlurRad = 8.0;

        //separation ; blur radius
        Vector2 boxBlurParams = (Vector2) {1.0, 7.0};

        // separation ; size;
        Vector2 dilationParams = (Vector2) {1.0, 0.0};

    BoxBlurDof();
    ~BoxBlurDof();
    void shaderScreenTex(Lights* lightShader);
    void shaderBlur();
    void shaderDilation();
    void shaderDoF();
    void drawUI(Vector3* sunlightPos);
    void loadTextures();
    void unloadTextures();
    void render(Lights* lights);
};

#endif