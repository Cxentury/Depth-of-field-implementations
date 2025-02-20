#ifndef UTILS_H
#define UTILS_H

#include <raylib.h>
#include <array>
#include <iostream>
#include <iterator>
#include <direct.h> 
#include <rlgl.h>
#include <unistd.h>

#include "rlImGui.h"
#include "imgui.h"
#include "raymath.h"

#define GLSL_VERSION 430
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif
//Class with things commmon to all dof techniques
class Utils{
    private:
    public:
    
    enum{
        DOF_BOXBLUR = 0,
        DOF_ACCUMULATION ,
        DOF_SINGLEPASS,
        DOF_GATHER_BASED
    };
    
        static RenderTexture2D LoadRenderTextureRGBA16(int width, int height);
        static RenderTexture2D LoadRenderTextureRGB8(int width, int height);
        static Texture2D background;
        static int sScreen_width;
        static int sScreen_height;
        static int sTechnique;
        static bool sAnimation;
        static bool sSphereMov;
        static float sAnimationSpeed;

        static RenderTexture2D sScreen_tex;
        static RenderTexture2D sCoC_tex;
        static Camera camera;
        static Model scene;
        static Model sphere;
        static Shader cocShader;
        static Color sClearColor;

        static std::array<Vector3,4> positions;

        // focus distance ; focus range
        static int lensSettingsLoc;
        static int screenTexLoc;
        static Vector2 lensParams;
        
        static Vector3 spherePos;
        static Vector3 cameraMov;
        static Vector3 cameraOriginalPos;

        static bool partialOcclusion;
        static bool depthDiscontinuity;
        static bool pixelBleeding;
        static bool splittingLayers;

        static void init();
        static void drawDepth();
        static void drawCoC();
        static void onResize();
        static void draw_scene();
        static void loadScreenAndDepthTex();
        static void unloadScreenAndDepthTex();
        static void unloadTextures();
        static void drawUI();
        static void drawUISimple();
        static void drawUIShowcase();
};
#endif