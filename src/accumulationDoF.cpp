#include "accumulationDoF.h"

#include <glad.h>

#define POISSON_PROGRESS_INDICATOR 1
#include "PoissonGenerator.h"

std::vector<PoissonGenerator::Point> samplePoints;

AccumulationDoF::AccumulationDoF(/* args */)
{
    generateSamples();
    SetRandomSeed((unsigned int)time(NULL));
    for (int i = 0; i < sampleCount; i++)
    {
        samples[i] = (Vector3) {(double(rand()) / RAND_MAX - 0.5),(double(rand()) / RAND_MAX - 0.5),0};
    }
}

AccumulationDoF::~AccumulationDoF(){
    UnloadShader(accumulationShader);
}

void AccumulationDoF::generateSamples(){
    PoissonGenerator::DefaultPRNG PRNG;
    uint32_t gridSize = sampleCount;
    samplePoints = PoissonGenerator::generatePoissonPoints(gridSize,PRNG);
}

void AccumulationDoF::render(Lights* lights){

    if(Utils::sAnimation){
        focusTarget.z= -14.6 * (sin(GetTime())+ 1.0) /2  * Utils::sAnimationSpeed;
    }
    if(Utils::depthDiscontinuity){
        focusTarget.z= 20;
    }

    Vector3 cameraPos = Utils::camera.position;
    Utils::camera.target = {0,0,0};
    
    Vector3 cameraForward = Vector3Normalize(Utils::camera.target - Utils::camera.position);
    Vector3 cameraRight = Vector3CrossProduct(Utils::camera.up, cameraForward);
    
    Utils::camera.target = Vector3Add(Utils::camera.target, cameraForward * focusTarget.z);

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

        UpdateCamera(&Utils::camera, CAMERA_PERSPECTIVE);

        BeginTextureMode(Utils::sScreen_tex);
            ClearBackground(Utils::sClearColor);
            BeginBlendMode(RL_BLEND_ALPHA);
                // BeginBlendMode(BLEND_ALPHA);
                // rlDisableColorBlend();
                Utils::draw_scene();
            EndBlendMode();
            // EndBlendMode();
            // rlEnableColorBlend();
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
        BeginShaderMode(accumulationShader);
            SetShaderValueTexture(accumulationShader,accumulatedTexLoc,Utils::sCoC_tex.texture);
            SetShaderValue(accumulationShader,nbSamplesLoc,&sampleCount,RL_SHADER_UNIFORM_INT);
            DrawTextureRec(Utils::sCoC_tex.texture, (Rectangle){ 0, 0, (float)Utils::sCoC_tex.texture.width, (float)-Utils::sCoC_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
        EndShaderMode();

        rlImGuiBegin();
            Utils::drawUISimple();
            drawUI(&lights->sunlightPos);
        rlImGuiEnd();
        DrawFPS(10, 10);
    EndDrawing();
    // rlDisableColorBlend();
}

void AccumulationDoF::drawUI(Vector3* sunlightPos){
    ImGui::Begin("Accumulation settings");
    ImGui::SliderInt("Poisson / Random distribution",&randomSampling, 0,1);
    if (ImGui::SliderInt("Number of Samples",&sampleCount, 1,499)){
        generateSamples();
    };
    ImGui::SliderFloat("Offset factor",&offsetFactor, 0.1f,3.0f);
    ImGui::SliderFloat3("Sunlight Position",&sunlightPos->x, 0.5f,15.0f);
    ImGui::SliderFloat3("Focus target",&focusTarget.x, -20.0f,60.0f);
    ImGui::End();
}
