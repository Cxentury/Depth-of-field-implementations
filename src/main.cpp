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
    rlSetClipPlanes(0.1,1000);
    
    BoxBlurDof boxBlurDof = BoxBlurDof();
    AccumulationDoF accumulationDoF = AccumulationDoF();
    Lights lights = Lights();

    Utils::init();
    Utils::camera.position = (Vector3){ 1.45334f, 0.723842f, 25.61f};
    Utils::camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    Utils::camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    Utils::camera.fovy = 27.0f;
    Utils::camera.projection = CAMERA_PERSPECTIVE;
    
    for(int i = 0; i < Utils::scene.materialCount; i++){
        Utils::scene.materials[i].shader = lights.lightShader;
    }
    
    //--------------------------------------------------------------------------------------

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    rlImGuiSetup(true);
    //--------------------------------------------------------------------------------------
    DisableCursor();    

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
        if(Utils::sTechnique == Utils::DOF_BOXBLUR)
            boxBlurDof.render(&lights);
        else if(Utils::sTechnique == Utils::DOF_ACCUMULATION)
            accumulationDoF.render(&lights);

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


