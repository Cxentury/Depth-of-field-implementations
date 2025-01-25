#include "BoxBlurDof.h"

void BoxBlurDof::shaderScreenTex(Light* lights){
    BeginShaderMode(shaders[SHADER_LIGHTS]);
        rlDisableColorBlend();
        for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shaders[SHADER_LIGHTS], lights[i]);
        SetShaderValue(shaders[SHADER_LIGHTS],lensSettingsLoc,&lensParams.x,SHADER_UNIFORM_VEC2);
        SetShaderValue(shaders[SHADER_LIGHTS], ambientLoc, (float[4]){ 0.1f, 0.1f, 0.1f, 1.0f }, SHADER_UNIFORM_VEC4);
        // for (int i = 0; i < MAX_LIGHTS; i++)
        // {
        //     if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
        //     else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
        // }
        Utils::draw_scene();
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
            DrawTextureRec(Utils::sScreen_tex.texture, (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
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
        SetShaderValueTexture(shaders[SHADER_DOF],cocTexLoc,Utils::sScreen_tex.texture);
        SetShaderValueTexture(shaders[SHADER_DOF],blurredTexLoc,textures[DILATION_TEX].texture);
        SetShaderValue(shaders[SHADER_DOF],blurRadLoc,&boxBlurParams.y,SHADER_UNIFORM_FLOAT);
        DrawTextureRec(textures[DILATION_TEX].texture, (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
    EndShaderMode();
}

void BoxBlurDof::drawUI(){
    rlImGuiBegin();	
        ImGui::Begin("Dof settings");
        ImGui::SliderFloat2("Focus distance ; Focus range",&lensParams.x, 0.0f,30.0f);
        // ImGui::SliderFloat("Max blur Radius",maxBlurRad, 0.0f,20.0f);
        ImGui::SliderFloat("Size Dilation",&dilationParams.y, 1.0f,20.0f);
        // ImGui::SliderFloat3("Cube distance",&position.x, -10.0f,30.0f);
        ImGui::End();       
    rlImGuiEnd();
    DrawFPS(10, 10);
}

void BoxBlurDof::loadTextures(){
    textures[BLUR_TEX] = LoadRenderTexture(Utils::sScreen_width, Utils::sScreen_height);
    textures[DILATION_TEX] = LoadRenderTexture(Utils::sScreen_width, Utils::sScreen_height);
}

void BoxBlurDof::unloadTextures(){
    UnloadRenderTexture(textures[BLUR_TEX]);
    UnloadRenderTexture(textures[DILATION_TEX]);   
}