#include "BoxBlurDof.h"

BoxBlurDof::BoxBlurDof(){

    shaders[SHADER_BLUR] = LoadShader(0, TextFormat("./src/shaders/box_blur.fs", GLSL_VERSION));
    shaders[SHADER_DILATION] = LoadShader(0, TextFormat("./src/shaders/dilation.fs", GLSL_VERSION));
    shaders[SHADER_DOF] = LoadShader(0, TextFormat("./src/shaders/coc_blur.fs", GLSL_VERSION));

    boxBlurParamsLoc = GetShaderLocation(shaders[SHADER_BLUR], "box_blur_settings");
    boxBlurScreenTexLoc = GetShaderLocation(shaders[SHADER_BLUR], "screen_texture");
    
    dilationScreenTexLoc = GetShaderLocation(shaders[SHADER_DILATION], "screen_texture");
    dilationParamsLoc = GetShaderLocation(shaders[SHADER_DILATION], "dilation_settings");

    blurRadLoc = GetShaderLocation(shaders[SHADER_DOF], "max_blur_radius");
    cocTexLoc = GetShaderLocation(shaders[SHADER_DOF], "screen_texture");
    blurredTexLoc = GetShaderLocation(shaders[SHADER_DOF], "blurred_texture");

    loadTextures();
    
}

BoxBlurDof::~BoxBlurDof(){
    UnloadRenderTexture(textures[BLUR_TEX]);
    UnloadRenderTexture(textures[DILATION_TEX]);   
    UnloadShader(shaders[SHADER_BLUR]);
    UnloadShader(shaders[SHADER_DILATION]);
    UnloadShader(shaders[SHADER_DOF]);
}

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

void BoxBlurDof::drawUI(Vector3* sunlightPos){

    ImGui::Begin("Box blur settings");
    ImGui::SliderFloat("Max blur Radius",&boxBlurParams.y, 1.0f,20.0f);
    ImGui::SliderFloat2("Separation ; Size Dilation",&dilationParams.x, .0f,15.0f);
    ImGui::SliderFloat3("Sunlight Position",&sunlightPos->x, 0.5f,15.0f);
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
    lights->updateShaderValues(true);
    
    BeginTextureMode(Utils::sScreen_tex);
        rlDisableColorBlend();
        ClearBackground(Utils::sClearColor);
        Utils::draw_scene();
        rlEnableColorBlend();
    EndTextureMode();

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
            drawUI(&lights->sunlightPos);
        rlImGuiEnd();
        DrawFPS(10, 10);
    EndDrawing();
}