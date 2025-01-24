/*******************************************************************************************
*
*   raylib [core] example - Basic 3d example
*
*   Welcome to raylib!
*
*   To compile example, just press F5.
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/
#include "main.h"
#include "rlImGui.h"
#include "imgui.h"

#define GLSL_VERSION 330
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
Camera camera = { 0 };
Vector3 cubePosition = { 0 };
std::array<Vector3,3> positions = {(Vector3) { 1.0f, 4.0f, -3.0f},(Vector3){3.0f, 2.0f, -6.0f},(Vector3){-4.0f, 6.0f, -12.0f}};
//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
// static void UpdateDrawFrame(void);          // Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int screenWidth = 800;
int screenHeight = 450;

using namespace std;
#include "rlgl.h"
#include <unistd.h>

RenderTexture2D LoadRenderTextureRGBA16(int width, int height)
{
    RenderTexture2D target = { 0 };

    target.id = rlLoadFramebuffer(); // Load an empty framebuffer

    if (target.id > 0)
    {
        rlEnableFramebuffer(target.id);

        // Create color texture (default to RGBA)
        target.texture.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R32G32B32A32, 1);
        target.texture.width = width;
        target.texture.height = height;
        target.texture.format = PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;
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


int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "raylib");
    camera.position = (Vector3){ 0.0f, 3.0f, 10.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    rlSetClipPlanes(0.1,100);
    //--------------------------------------------------------------------------------------

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    rlImGuiSetup(true);
    //--------------------------------------------------------------------------------------

    Shader coc_tex_shader = LoadShader(0, TextFormat("./src/shaders/coc_texture.fs", GLSL_VERSION));
    Shader coc_blur_shader = LoadShader(0, TextFormat("./src/shaders/coc_blur.fs", GLSL_VERSION));
    Shader box_blur_shader = LoadShader(0, TextFormat("./src/shaders/box_blur.fs", GLSL_VERSION));

    //Z_buffer and clean texture
    RenderTexture2D z_buffer_tex = LoadRenderTextureRGBA16(screenWidth, screenHeight);
    RenderTexture2D box_blur_tex = LoadRenderTexture(screenWidth, screenHeight);

    float focusDistance = 8.5;
    float focusRange = .3;
    // float maxBlurRad = 8.0;
    // float separation = 1;
    //maxBlurRad ; separation
    Vector2 boxBlurParams = (Vector2) {0.0, 0.0};

    int focusDistLoc = GetShaderLocation(coc_tex_shader, "focus_distance");
    int focusRangeLoc = GetShaderLocation(coc_tex_shader, "focus_range");

    int blurRadLoc = GetShaderLocation(coc_blur_shader, "max_blur_radius");
    int cocTexLoc = GetShaderLocation(coc_blur_shader, "coc_texture");


    int boxBlurParamsLoc = GetShaderLocation(box_blur_shader, "box_blur_settings");
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if(IsWindowResized()){
            UnloadRenderTexture(z_buffer_tex);
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();
            z_buffer_tex = LoadRenderTextureRGBA16(screenWidth, screenHeight);
            sleep(.1);
        }
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&camera, CAMERA_PERSPECTIVE);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginTextureMode(z_buffer_tex);
            ClearBackground(RAYWHITE);
            BeginShaderMode(coc_tex_shader);
                rlDisableColorBlend();
                SetShaderValue(coc_tex_shader,focusDistLoc,&focusDistance,SHADER_UNIFORM_FLOAT);
                SetShaderValue(coc_tex_shader,focusRangeLoc,&focusRange,SHADER_UNIFORM_FLOAT);
                draw_scene();
                rlEnableColorBlend();
            EndShaderMode();
        EndTextureMode();
        
        BeginTextureMode(box_blur_tex);
            ClearBackground(RAYWHITE);
            BeginShaderMode(box_blur_shader);
                SetShaderValue(box_blur_shader,boxBlurParamsLoc,&boxBlurParams.x,SHADER_UNIFORM_VEC2);
            EndShaderMode();
        EndTextureMode();

        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginShaderMode(coc_blur_shader);
                SetShaderValueTexture(coc_blur_shader,cocTexLoc,z_buffer_tex.texture);
                SetShaderValue(coc_blur_shader,blurRadLoc,&boxBlurParams.y,SHADER_UNIFORM_FLOAT);
                DrawTextureRec(z_buffer_tex.texture, (Rectangle){ 0, 0, (float)z_buffer_tex.texture.width, (float)-z_buffer_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();

            rlImGuiBegin();	
                DoFParameters(&focusDistance, &focusRange, &boxBlurParams.y,&positions[0]);
            rlImGuiEnd();
            DrawFPS(10, 10);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(z_buffer_tex);
    rlImGuiShutdown();
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


void draw_scene(){
    BeginMode3D(camera);
        DrawCube(cubePosition, 2.0f, 2.0f, 2.0f, RED);
        DrawCube(positions[0], 1.0f, 4.0f, 3.0f, RED);
        DrawCube(positions[1], 3.0f, 2.0f, 6.0f, RED);
        DrawSphere(positions[2],4,BLUE);
        DrawGrid(10, 1.0f);
    EndMode3D();
}
 
void DoFParameters(float *focusDistance,float *focusRange,float* maxBlurRad, Vector3 *position)
{
    ImGui::Begin("Dof settings");
    ImGui::SliderFloat("Focus distance",focusDistance, 0.0f,30.0f);
    ImGui::SliderFloat("Focus range",focusRange, 0.0f,30.0f);
    ImGui::SliderFloat("Max blur Radius",maxBlurRad, 0.0f,20.0f);
    ImGui::SliderFloat3("Cube distance",&position->x, 0.0f,30.0f);
    ImGui::End();
}
