#include <iostream>
#include "utils.h"


int Utils::sScreen_width = 800;
int Utils::sScreen_height = 450;

Camera Utils::camera = { 0 };
RenderTexture2D Utils::sScreen_tex = {0};
Texture2D Utils::background = {0};
Model Utils::scene = {0};

std::array<Vector3,4> Utils::positions = {(Vector3){0} ,(Vector3) { 1.0f, 4.0f, -3.0f},(Vector3){3.0f, 2.0f, -6.0f},(Vector3){-4.0f, 6.0f, -12.0f}};

void Utils::init(){
    loadScreenTex();
    Utils::background = LoadTexture("./images/Medieval city by A.Rocha.png");
    Utils::scene = LoadModel("./scene/scene.obj");
}

RenderTexture2D Utils::LoadRenderTextureRGBA16(int width, int height)
{
    RenderTexture2D target = { 0 };

    target.id = rlLoadFramebuffer(); // Load an empty framebuffer

    if (target.id > 0)
    {
        rlEnableFramebuffer(target.id);

        // Create color texture (default to RGBA)
        target.texture.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R16G16B16A16, 1);
        target.texture.width = width;
        target.texture.height = height;
        target.texture.format = PIXELFORMAT_UNCOMPRESSED_R16G16B16A16;
        target.texture.mipmaps = 1;

        // Create depth renderbuffer/texture
        target.depth.id = rlLoadTextureDepth(width, height, true);
        target.depth.width = width;
        target.depth.height = height;
        target.depth.format = 19;       //DEPTH_COMPONENT_24BIT?
        target.depth.mipmaps = 1;

        // Attach color texture and depth renderbuffer/texture to FBO
        rlFramebufferAttach(target.id, target.texture.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(target.id, target.depth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);

        // Check if fbo is complete with attachments (valid)
        if (rlFramebufferComplete(target.id)) TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);

        rlDisableFramebuffer();
    }
    else TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");

    return target;
}

void Utils::loadScreenTex(){
    Utils::sScreen_tex = LoadRenderTextureRGBA16(Utils::sScreen_width, Utils::sScreen_height);
}

void Utils::unloadScreenTex(){
    UnloadRenderTexture(Utils::sScreen_tex);
}

void Utils::unloadTextures(){
    UnloadRenderTexture(Utils::sScreen_tex);
    UnloadTexture(Utils::background);
    UnloadModel(Utils::scene);
}

void Utils::draw_scene(){
    BeginMode3D(camera);
        DrawModel(Utils::scene, {0,0,0}, 1.2, WHITE);
        // DrawCube(positions[0], 2.0f, 2.0f, 2.0f, RED);
        // DrawCube(positions[1], 3.0f, 2.0f, 6.0f, YELLOW);
        // DrawCube(positions[2], 1.0f, 4.0f, 3.0f, GREEN);
        // DrawSphere(positions[3],4,BLUE);
        DrawGrid(10, 1.0f);

        // for (int i = 0; i < MAX_LIGHTS; i++)
        // {
        //     if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
        //     else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
        // }

    EndMode3D();
}

void Utils::onResize(){
    Utils::unloadScreenTex();
    Utils::sScreen_width = GetScreenWidth();
    Utils::sScreen_height = GetScreenHeight();
    std::cout << Utils::sScreen_width << std::endl;
    std::cout << Utils::sScreen_height << std::endl;

    Utils::loadScreenTex();
}
