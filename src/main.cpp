#include "main.h"

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
// static void UpdateDrawFrame(void);          // Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "raylib");
    rlSetClipPlanes(0.1,100);
    Utils::init();
    Utils::camera.position = (Vector3){ 0.0f, 0.0f, 17.5f };
    Utils::camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    Utils::camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    Utils::camera.fovy = 60.0f;
    Utils::camera.projection = CAMERA_PERSPECTIVE;
    //--------------------------------------------------------------------------------------

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    rlImGuiSetup(true);
    //--------------------------------------------------------------------------------------
DisableCursor();    
    
    BoxBlurDof boxBlurDof = BoxBlurDof();
    for(int i = 0; i < Utils::scene.materialCount; i++){
        Utils::scene.materials[i].shader = boxBlurDof.shaders[SHADER_LIGHTS];
    }

    //Z_buffer and clean texture
    Light lights[MAX_LIGHTS] = { 0 };
    lights[0] = CreateLight(LIGHT_POINT, (Vector3){ -2, 1, -2 }, Vector3Zero(), YELLOW, boxBlurDof.shaders[SHADER_LIGHTS]);
    lights[1] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, 2 }, Vector3Zero(), RED, boxBlurDof.shaders[SHADER_LIGHTS]);
    lights[2] = CreateLight(LIGHT_POINT, (Vector3){ -2, 1, 2 }, Vector3Zero(), GREEN, boxBlurDof.shaders[SHADER_LIGHTS]);
    // lights[3] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, -2 }, Vector3Zero(), BLUE, boxBlurDof.shaders[SHADER_LIGHTS]);
    lights[3] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, 18 }, Vector3Zero(), RED, boxBlurDof.shaders[SHADER_LIGHTS]);
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if(IsWindowResized()){
            boxBlurDof.unloadTextures();
            Utils::onResize();
            boxBlurDof.loadTextures();
            sleep(.1);
        }
        // Update
        //----------------------------------------------------------------------------------
        UpdateCamera(&Utils::camera, CAMERA_PERSPECTIVE);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginTextureMode(Utils::sScreen_tex);
            ClearBackground(RAYWHITE);
            rlDisableDepthTest();
                DrawTexturePro(Utils::background, (Rectangle){ 0, 0, (float)Utils::background.width, (float)Utils::background.height },
                (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height },(Vector2){ 0, 0 }, 0,WHITE);
            rlEnableDepthTest();

            boxBlurDof.shaderScreenTex(lights);
        EndTextureMode();

        boxBlurDof.shaderBlur();
        boxBlurDof.shaderDilation();
        
        BeginDrawing();
            ClearBackground(RAYWHITE);
            boxBlurDof.shaderDoF();
            boxBlurDof.drawUI();
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    boxBlurDof.unloadTextures();
    Utils::unloadTextures();
    rlImGuiShutdown();
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


