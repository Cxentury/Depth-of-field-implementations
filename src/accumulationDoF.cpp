#include "accumulationDoF.h"
#include "glad.h"

AccumulationDoF::AccumulationDoF(/* args */)
{
    SetRandomSeed((unsigned int)time(NULL));
    for (int i = 0; i < 1000; i++)
    {
        samples[i] = (Vector3) {(double(rand()) / RAND_MAX - 0.5),(double(rand()) / RAND_MAX - 0.5),0};
    }
}

AccumulationDoF::~AccumulationDoF(){
    UnloadShader(accumulationShader);
}

void AccumulationDoF::render(Lights* lights){
    
    Vector3 cameraPos = Utils::camera.position;
    lights->updateShaderValues(false);
    // BeginTextureMode(Utils::sCoC_tex);
    //     ClearBackground(BLACK);
    //     Utils::draw_scene();
    //     rlColor4f(1.,1.,1.,aplhaFac);
    // EndTextureMode();
    BeginTextureMode(Utils::sCoC_tex);
    
    ClearBackground(BLACK);
    EndTextureMode();

    int sampleLoop = 0;
    for (; sampleLoop < sampleCount; sampleLoop++)
    {

        Utils::camera.position= cameraPos + samples[sampleLoop]*offsetFactor;
        Utils::camera.target = {0,0,0};
        UpdateCamera(&Utils::camera, CAMERA_PERSPECTIVE);

        BeginTextureMode(Utils::sScreen_tex);
            rlDisableColorBlend();
            ClearBackground(BLACK);
            DrawTexturePro(Utils::background, (Rectangle){ 0, 0, (float)Utils::background.width, (float)Utils::background.height },
            (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height },(Vector2){ 0, 0 }, 0,WHITE);
            Utils::draw_scene();
            rlEnableColorBlend();
        EndTextureMode();

        BeginTextureMode(Utils::sCoC_tex);
            rlSetBlendFactors(GL_CONSTANT_ALPHA,GL_ONE,GL_FUNC_SUBTRACT);
            rlEnableColorBlend();
            BeginBlendMode(RL_BLEND_ADDITIVE);
            // glBlendColor(0,0,0,1.0/alphaFac);
            DrawTextureRec(Utils::sScreen_tex.texture, (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height, }, (Vector2){ 0, 0 },WHITE);
            EndBlendMode();
        EndTextureMode();
    }

    
    Utils::camera.position = cameraPos;
    BeginDrawing();
        ClearBackground(BLACK);
        // rlSetBlendMode(RL_BLEND_ALPHA);
        // rlEnableColorBlend();
        
        BeginShaderMode(accumulationShader);
            SetShaderValueTexture(accumulationShader,accumulatedTexLoc,Utils::sCoC_tex.texture);
            SetShaderValue(accumulationShader,nbSamplesLoc,&sampleCount,RL_SHADER_UNIFORM_INT);
            DrawTextureRec(Utils::sCoC_tex.texture, (Rectangle){ 0, 0, (float)Utils::sCoC_tex.texture.width, (float)-Utils::sCoC_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
        EndShaderMode();

        rlImGuiBegin();	
            Utils::drawUI();
            drawUI();
        rlImGuiEnd();
        DrawFPS(10, 10);
    EndDrawing();
    // rlDisableColorBlend();
}

void AccumulationDoF::drawUI(){
    ImGui::Begin("Accumulation settings");
    ImGui::SliderInt("Number of Samples",&sampleCount, 0,500);
    ImGui::SliderFloat("Offset factor",&offsetFactor, 0.01f,1.0f);
    ImGui::End();        
}
