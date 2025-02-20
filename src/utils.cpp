#include <iostream>
#include "utils.h"


int Utils::sScreen_width = 800;
int Utils::sScreen_height = 450;

// Color Utils::sClearColor = {138, 155, 192, 255};
Color Utils::sClearColor = {133, 154, 229, 255};

Camera Utils::camera = { 0 };
RenderTexture2D Utils::sScreen_tex = {0};
RenderTexture2D Utils::sCoC_tex = {0};
Shader Utils::cocShader = {0};
Texture2D Utils::background = {0};

Vector2 Utils::lensParams = (Vector2) {23.2, 9};
Vector3 Utils::spherePos = (Vector3) {1.154, 1.6, 0};

int Utils::lensSettingsLoc = 0;
int Utils::screenTexLoc = 0;
int Utils::sTechnique = DOF_BOXBLUR;
bool Utils::sAnimation = false;
bool Utils::sSphereMov = false;
float Utils::sAnimationSpeed = 1.0;

Model Utils::scene = {0};
Model Utils::sphere = {0};

std::array<Vector3,4> Utils::positions = {(Vector3){0} ,(Vector3) { 1.0f, 4.0f, -3.0f},(Vector3){3.0f, 2.0f, -6.0f},(Vector3){-4.0f, 6.0f, -12.0f}};

void Utils::init(){
    loadScreenAndDepthTex();
    Utils::background = LoadTexture("./images/Medieval city by A.Rocha.png");
    // Utils::scene = LoadModel("./scene/scene1.obj");
    Utils::scene = LoadModel("./scene/scene1.obj");
    Utils::sphere = LoadModel("./scene/sphere.obj");
    Utils::cocShader = LoadShader(0,"./src/shaders/coc.fs");
    Utils::lensSettingsLoc = GetShaderLocation(cocShader, "lens_settings");
    Utils::screenTexLoc = GetShaderLocation(cocShader, "scren_texture");
}

RenderTexture2D Utils::LoadRenderTextureRGB8(int width, int height)
{
    RenderTexture2D target = { 0 };

    target.id = rlLoadFramebuffer(); // Load an empty framebuffer

    if (target.id > 0)
    {
        rlEnableFramebuffer(target.id);

        target.texture.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8, 1);
        target.texture.width = width;
        target.texture.height = height;
        target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
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

void Utils::loadScreenAndDepthTex(){
    Utils::sScreen_tex = LoadRenderTextureRGBA16(Utils::sScreen_width, Utils::sScreen_height);
    Utils::sCoC_tex = LoadRenderTextureRGBA16(Utils::sScreen_width, Utils::sScreen_height);
}

void Utils::unloadScreenAndDepthTex(){
    UnloadRenderTexture(Utils::sScreen_tex);
    UnloadRenderTexture(Utils::sCoC_tex);
}

void Utils::unloadTextures(){
    UnloadRenderTexture(Utils::sScreen_tex);
    UnloadRenderTexture(Utils::sCoC_tex);
    UnloadShader(Utils::cocShader);
    UnloadTexture(Utils::background);
    UnloadModel(Utils::scene);
}

void Utils::draw_scene(){
    BeginMode3D(Utils::camera);
        DrawModel(Utils::sphere, spherePos,1,WHITE);
        // DrawSphereEx(spherePos, 0.2f, 8, 8, RED);
        DrawModel(Utils::scene, {0,0,0}, 1.2, WHITE);
    EndMode3D();
}

void Utils::drawCoC(){
    BeginShaderMode(cocShader);
        SetShaderValue(cocShader,lensSettingsLoc,&Utils::lensParams.x,SHADER_UNIFORM_VEC2);
        SetShaderValueTexture(cocShader,screenTexLoc,Utils::sScreen_tex.texture);
        DrawTextureRec(Utils::sCoC_tex.texture, (Rectangle){ 0, 0, (float)Utils::sCoC_tex.texture.width, (float)-Utils::sCoC_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
    EndShaderMode();
}

void Utils::onResize(){
    Utils::unloadScreenAndDepthTex();
    Utils::sScreen_width = GetScreenWidth();
    Utils::sScreen_height = GetScreenHeight();
    std::cout << Utils::sScreen_width << std::endl;
    std::cout << Utils::sScreen_height << std::endl;
    Utils::loadScreenAndDepthTex();
}

void Utils::drawUI(){
    if(sAnimation)
        lensParams.x=8.5 * sin(GetTime() * sAnimationSpeed) + 18.5;
    if(sSphereMov){
        spherePos.z= 19.6 * (sin(GetTime())+ 1.0) /2  * Utils::sAnimationSpeed;
    }

    ImGui::Begin("DoF settings");
    ImGui::SliderFloat3("Sphere Pos", &Utils::spherePos.x, -30,30);
    ImGui::SliderInt("Technique",&Utils::sTechnique, 0,3);
    ImGui::SliderFloat2("Focus distance ; Focus range",&Utils::lensParams.x, 0.0f,120.0f);
    ImGui::Checkbox("Animation",&Utils::sAnimation);
    ImGui::Checkbox("Sphere move",&Utils::sSphereMov);
    ImGui::SliderFloat("Animation speed", &Utils::sAnimationSpeed, .1,2);
    ImGui::End();       
}

void Utils::drawUISimple(){
    if(sAnimation)
        lensParams.x=8.5 * sin(GetTime() * sAnimationSpeed) + 18.5;
    if(sSphereMov){
        spherePos.z= 19.6 * (sin(GetTime())+ 1.0) /2  * Utils::sAnimationSpeed;
    }
    ImGui::Begin("DoF settings");
    ImGui::SliderInt("Technique",&Utils::sTechnique, 0,3);
    ImGui::Checkbox("Animation",&Utils::sAnimation);
    ImGui::Checkbox("Sphere move",&Utils::sSphereMov);
    ImGui::SliderFloat("Animation speed", &Utils::sAnimationSpeed, .1,2);
    ImGui::End();       
}
