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

#include "rlights.h"


class Utils{
    private:
        static RenderTexture2D LoadRenderTextureRGBA16(int width, int height);
    public:
        static Texture2D background;
        static int sScreen_width;
        static int sScreen_height;
        
        static RenderTexture2D sScreen_tex;
        static Camera camera;
        static std::array<Vector3,4> positions;
        static Model scene;

        static void init();
        static void onResize();
        static void draw_scene();
        static void loadScreenTex();
        static void unloadScreenTex();
        static void unloadTextures();
};
#endif