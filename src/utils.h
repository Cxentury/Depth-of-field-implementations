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

//Class with things commmon to all dof techniques
class Utils{
    private:
        static RenderTexture2D LoadRenderTextureRGBA16(int width, int height);
    public:
        static Texture2D background;
        static int sScreen_width;
        static int sScreen_height;
        
        static RenderTexture2D sScreen_tex;
        static RenderTexture2D sCoC_tex;

        static Camera camera;
        static Model scene;
        static Shader cocShader;
        static std::array<Vector3,4> positions;

        // focus distance ; focus range
        static int lensSettingsLoc;
        static int screenTexLoc;

        static Vector2 lensParams;
        
        static void init();
        static void drawDepth();
        static void drawCoC();
        static void onResize();
        static void draw_scene();
        static void loadScreenAndDepthTex();
        static void unloadScreenAndDepthTex();
        static void unloadTextures();
        static void drawUI();
};
#endif