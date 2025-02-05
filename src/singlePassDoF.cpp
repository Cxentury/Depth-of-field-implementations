#include "SinglePassDoF.h"

SinglePassDoF::SinglePassDoF(/* args */)
{
    SinglePassDoFShader = LoadShader(0, "./src/shaders/singlepass/single_pass.fs");
    screenTexLoc = GetShaderLocation(SinglePassDoFShader,"screen_texture");
    lensSettingsLoc = GetShaderLocation(SinglePassDoFShader,"lens_settings");
    farPlaneLoc = GetShaderLocation(SinglePassDoFShader,"far_plane");
}

SinglePassDoF::~SinglePassDoF()
{
    UnloadShader(SinglePassDoFShader);
}

void SinglePassDoF::drawUI(Vector3* sunlightPos){

    ImGui::Begin("Box blur settings");
    // ImGui::SliderFloat("Max blur Radius",maxBlurRad, 0.0f,20.0f);
    ImGui::SliderFloat3("Sunlight Position",&sunlightPos->x, 0.5f,15.0f);
    // ImGui::SliderFloat3("Cube distance",&position.x, -10.0f,30.0f);
    ImGui::End();       

}

void SinglePassDoF::shaderDoF(){
    BeginShaderMode(SinglePassDoFShader);
        SetShaderValueTexture(SinglePassDoFShader,screenTexLoc,Utils::sScreen_tex.texture);
        SetShaderValue(SinglePassDoFShader,lensSettingsLoc,&Utils::lensParams,RL_SHADER_UNIFORM_VEC2);
        // SetShaderValue(SinglePassDoFShader,farPlaneLoc,1000,RL_SHADER_UNIFORM_FLOAT);
        DrawTextureRec(Utils::sScreen_tex.texture, (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
    EndShaderMode();
}

void SinglePassDoF::render(Lights* lights){
    lights->updateShaderValues(true);
    
    BeginTextureMode(Utils::sScreen_tex);
        rlDisableColorBlend();
        ClearBackground(Utils::sClearColor);
        Utils::draw_scene();
        rlEnableColorBlend();
    EndTextureMode();

    BeginDrawing();
        ClearBackground(RAYWHITE);
        shaderDoF();
        rlImGuiBegin();	
            Utils::drawUI();
            drawUI(&lights->sunlightPos);
        rlImGuiEnd();
        DrawFPS(10, 10);
    EndDrawing();
}