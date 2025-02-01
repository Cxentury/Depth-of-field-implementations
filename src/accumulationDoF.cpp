#include "accumulationDoF.h"

#include <glad.h>

#define POISSON_PROGRESS_INDICATOR 1
#include "PoissonGenerator.h"

std::vector<PoissonGenerator::Point> samplePoints;

AccumulationDoF::AccumulationDoF(/* args */)
{
    PoissonGenerator::DefaultPRNG PRNG;
    uint32_t gridSize = sampleCount;
    samplePoints = PoissonGenerator::generateJitteredGridPoints(gridSize,PRNG,false,0.02f);
    
    SetRandomSeed((unsigned int)time(NULL));
    for (int i = 0; i < sampleCount; i++)
    {
        samples[i] = (Vector3) {(double(rand()) / RAND_MAX - 0.5),(double(rand()) / RAND_MAX - 0.5),0};
    }
}

AccumulationDoF::~AccumulationDoF(){
    UnloadShader(accumulationShader);
}

void AccumulationDoF::render(Lights* lights){

    Vector3 cameraPos = Utils::camera.position;

    Vector3 cameraForward = Vector3Normalize(Utils::camera.target - Utils::camera.position);
    Vector3 cameraRight = Vector3CrossProduct(Utils::camera.up, cameraForward);

    lights->updateShaderValues(false);
    BeginTextureMode(Utils::sCoC_tex);
    ClearBackground(BLANK);
    EndTextureMode();

    for (int i = 0; i < sampleCount; i++)
    {

        float jitterX;
        float jitterY;

        if(randomSampling){
            jitterX = samples[i].x * offsetFactor;
            jitterY = samples[i].y * offsetFactor;
        }
        else{
            jitterX = (samplePoints[i].x - 0.5f) * offsetFactor;
            jitterY = (samplePoints[i].y - 0.5f) * offsetFactor;
        }

        Vector3 offset = Vector3Add(cameraRight * jitterX, Utils::camera.up * jitterY);
        Utils::camera.position= Vector3Add(cameraPos, offset);
        Utils::camera.target = {0,0,0};

        UpdateCamera(&Utils::camera, CAMERA_PERSPECTIVE);

        BeginTextureMode(Utils::sScreen_tex);
            ClearBackground(Utils::sClearColor);
            rlDisableColorBlend();
            // DrawTexturePro(Utils::background, (Rectangle){ 0, 0, (float)Utils::background.width, (float)Utils::background.height },
            // (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height },(Vector2){ 0, 0 }, 0,WHITE);
            Utils::draw_scene();
            rlEnableColorBlend();
        EndTextureMode();

        BeginTextureMode(Utils::sCoC_tex);
            // rlSetBlendFactors(GL_CONSTANT_ALPHA,GL_ONE,GL_FUNC_SUBTRACT);
            // rlEnableColorBlend();
            BeginBlendMode(RL_BLEND_ADDITIVE);
            // glBlendColor(0,0,0,1.0/sampleCount);
            BeginShaderMode(passThroughShader);
                SetShaderValueTexture(passThroughShader,accumulatedTexLoc,Utils::sScreen_tex.texture);
                DrawTextureRec(Utils::sScreen_tex.texture, (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height, }, (Vector2){ 0, 0 },WHITE);
            EndShaderMode();

            EndBlendMode();
        EndTextureMode();
    }


    Utils::camera.position = cameraPos;
    BeginDrawing();
        ClearBackground(Utils::sClearColor);
        // rlSetBlendMode(RL_BLEND_ALPHA);
        // rlEnableColorBlend();

        // DrawTexturePro(Utils::background, (Rectangle){ 0, 0, (float)Utils::background.width, (float)Utils::background.height },
        // (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height },(Vector2){ 0, 0 }, 0,WHITE);
        BeginShaderMode(accumulationShader);
            SetShaderValueTexture(accumulationShader,accumulatedTexLoc,Utils::sCoC_tex.texture);
            SetShaderValue(accumulationShader,nbSamplesLoc,&sampleCount,RL_SHADER_UNIFORM_INT);
            DrawTextureRec(Utils::sCoC_tex.texture, (Rectangle){ 0, 0, (float)Utils::sCoC_tex.texture.width, (float)-Utils::sCoC_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
        EndShaderMode();

        rlImGuiBegin();
            Utils::drawUI();
            drawUI(&lights->sunlightPos);
        rlImGuiEnd();
        DrawFPS(10, 10);
    EndDrawing();
    // rlDisableColorBlend();
}

void AccumulationDoF::drawUI(Vector3* sunlightPos){
    ImGui::Begin("Accumulation settings");
    ImGui::SliderInt("Poisson / Random distribution",&randomSampling, 0,1);
    ImGui::SliderInt("Number of Samples",&sampleCount, 0,500);
    ImGui::SliderFloat("Offset factor",&offsetFactor, 0.01f,1.0f);
    ImGui::SliderFloat3("Sunlight Position",&sunlightPos->x, 0.5f,15.0f);
    ImGui::End();
}
