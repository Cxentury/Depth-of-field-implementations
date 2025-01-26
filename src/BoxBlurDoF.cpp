#include "BoxBlurDof.h"

void BoxBlurDof::shaderScreenTex(Lights* lightShader){
    BeginShaderMode(lightShader->lightShader);
        rlDisableColorBlend();
        DrawTextureRec(Utils::sScreen_tex.texture, (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
        // Utils::draw_scene();
        rlEnableColorBlend();
    EndShaderMode();
}

void BoxBlurDof::shaderBlur(){
    BeginTextureMode(textures[BLUR_TEX]);
        ClearBackground(RAYWHITE);
        BeginShaderMode(shaders[SHADER_BLUR]);
            SetShaderValue(shaders[SHADER_BLUR],boxBlurParamsLoc,&boxBlurParams.x,SHADER_UNIFORM_VEC2);
            SetShaderValueTexture(shaders[SHADER_BLUR],boxBlurScreenTexLoc,Utils::sScreen_tex.texture);
            //Does not work when I just draw a rect, even If I don't need the texture
            DrawTextureRec(Utils::sCoC_tex.texture, (Rectangle){ 0, 0, (float)Utils::sCoC_tex.texture.width, (float)-Utils::sCoC_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
        EndShaderMode();
    EndTextureMode();
}

void BoxBlurDof::shaderDilation(){
    BeginTextureMode(textures[DILATION_TEX]);
        ClearBackground(RAYWHITE);
        BeginShaderMode(shaders[SHADER_DILATION]);
            SetShaderValue(shaders[SHADER_DILATION],dilationParamsLoc,&dilationParams.x,SHADER_UNIFORM_VEC2);
            SetShaderValueTexture(shaders[SHADER_DILATION],dilationScreenTexLoc,textures[BLUR_TEX].texture);
            //Does not work when I just draw a rect, even If I don't need the texture
            DrawTextureRec(textures[BLUR_TEX].texture, (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
        EndShaderMode();
    EndTextureMode();
}

void BoxBlurDof::shaderDoF(){
    BeginShaderMode(shaders[SHADER_DOF]);
        SetShaderValueTexture(shaders[SHADER_DOF],cocTexLoc,Utils::sCoC_tex.texture);
        SetShaderValueTexture(shaders[SHADER_DOF],blurredTexLoc,textures[DILATION_TEX].texture);
        SetShaderValue(shaders[SHADER_DOF],blurRadLoc,&boxBlurParams.y,SHADER_UNIFORM_FLOAT);
        DrawTextureRec(textures[DILATION_TEX].texture, (Rectangle){ 0, 0, (float)Utils::sCoC_tex.texture.width, (float)-Utils::sCoC_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
    EndShaderMode();
}

void BoxBlurDof::drawUI(){

    ImGui::Begin("Box blur settings");
    // ImGui::SliderFloat("Max blur Radius",maxBlurRad, 0.0f,20.0f);
    ImGui::SliderFloat2("Separation ; Size Dilation",&dilationParams.x, 0.1f,15.0f);
    // ImGui::SliderFloat3("Cube distance",&position.x, -10.0f,30.0f);
    ImGui::End();       

}

void BoxBlurDof::loadTextures(){
    textures[BLUR_TEX] = LoadRenderTexture(Utils::sScreen_width, Utils::sScreen_height);
    textures[DILATION_TEX] = LoadRenderTexture(Utils::sScreen_width, Utils::sScreen_height);
}

void BoxBlurDof::unloadTextures(){
    UnloadRenderTexture(textures[BLUR_TEX]);
    UnloadRenderTexture(textures[DILATION_TEX]);   
}

void BoxBlurDof::render(Lights* lights){
    
    //Depth and screen texture
    BeginTextureMode(Utils::sCoC_tex);
        ClearBackground(RAYWHITE);
            rlDisableColorBlend();
            Utils::drawCoC();
            rlEnableColorBlend();
    EndTextureMode();

    shaderBlur();
    shaderDilation();
    BeginDrawing();
        ClearBackground(RAYWHITE);
        shaderDoF();

        rlImGuiBegin();	
            Utils::drawUI();
            drawUI();
        rlImGuiEnd();
        DrawFPS(10, 10);
    EndDrawing();
}