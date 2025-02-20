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
	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "raylib");
    rlSetClipPlanes(0.1,1000);
    // SetTraceLogLevel(LOG_ERROR);
    BoxBlurDof boxBlurDof = BoxBlurDof();
    AccumulationDoF accumulationDoF = AccumulationDoF();
    SinglePassDoF singlePassDoF = SinglePassDoF();
    GatherBasedDoF gatherBadedDoF = GatherBasedDoF();

    Lights lights = Lights();

    Utils::init();
    Utils::camera.position = (Vector3){ 1.45334f, 0.723842f, 25.61f};
    Utils::camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    Utils::camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    Utils::camera.fovy = 27.0f;
    Utils::camera.projection = CAMERA_PERSPECTIVE;
    
    Utils::sphere.materials[0].shader = lights.lightShader;

    for(int i = 0; i < Utils::scene.materialCount - 6 ; i++){
        Utils::scene.materials[i].shader = lights.lightShader;
    }

    // Last 6 are lights
    for(int i = Utils::scene.materialCount - 6; i < Utils::scene.materialCount ; i++){
        Utils::scene.materials[i].shader = lights.bypassLightShader;
    }
    //--------------------------------------------------------------------------------------

    SetTargetFPS(60);
    rlImGuiSetup(true);
    //--------------------------------------------------------------------------------------
    DisableCursor();    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if(IsWindowResized()){
            boxBlurDof.unloadTextures();
            gatherBadedDoF.unloadTextures();
            Utils::onResize();
            boxBlurDof.loadTextures();
            gatherBadedDoF.loadTextures();

            sleep(.1);
        }
        // Update
        //------------------------------------------------------w----------------------------
        UpdateCamera(&Utils::camera, CAMERA_PERSPECTIVE);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        if(Utils::sTechnique == Utils::DOF_BOXBLUR)
            boxBlurDof.render(&lights);
        else if(Utils::sTechnique == Utils::DOF_ACCUMULATION)
            accumulationDoF.render(&lights);
        else if(Utils::sTechnique == Utils::DOF_SINGLEPASS)
            singlePassDoF.render(&lights);
        else if(Utils::sTechnique == Utils::DOF_GATHER_BASED)
            gatherBadedDoF.render(&lights);
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    // boxBlurDof.unloadTextures();
    Utils::unloadTextures();
    boxBlurDof.unloadTextures();
    gatherBadedDoF.unloadTextures();
    rlImGuiShutdown();
    CloseWindow();                  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


