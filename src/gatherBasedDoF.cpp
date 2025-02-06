#include "GatherBasedDoF.h"

GatherBasedDoF::GatherBasedDoF(/* args */)
{   
    cocTex = Utils::LoadRenderTextureRGB8(Utils::sScreen_width, Utils::sScreen_height);
    DSTex = Utils::LoadRenderTextureRGB8(Utils::sScreen_width/4, Utils::sScreen_height/4);
    cocNearBlurred = Utils::LoadRenderTextureRGB8(Utils::sScreen_width/4, Utils::sScreen_height/4);
    
    downsamplePass = LoadMultiRenderTexture(Utils::sScreen_width/4, Utils::sScreen_height/4);

    shaderCoC = LoadShader(0, TextFormat("./src/shaders/practical_gather_based/coc.fs", GLSL_VERSION));
    shaderDS = LoadShader(0, TextFormat("./src/shaders/practical_gather_based/downsample.fs", GLSL_VERSION));
    shaderCoCNearMaxFilter = LoadShader(0, TextFormat("./src/shaders/practical_gather_based/nearCoCMaxFilter.fs", GLSL_VERSION));
    shaderCoCNearBlur = LoadShader(0, TextFormat("./src/shaders/practical_gather_based/nearCoCBlur.fs", GLSL_VERSION));
    
    shaderCoCTexLoc = GetShaderLocation(shaderCoC,"screen_texture");
    lensSettingsLoc = GetShaderLocation(shaderCoC, "lens_settings");
    
    shaderDsScreenLoc = GetShaderLocation(shaderDS, "screen_texture");
    shaderDsCoCLoc = GetShaderLocation(shaderDS, "coc_texture");

    shaderCoCNearTexLoc = GetShaderLocation(shaderCoCNearMaxFilter, "coc_texture");
    shaderCoCNearMaxTexLoc = GetShaderLocation(shaderCoCNearMaxFilter, "coc_texture_max");
    horizontalPassLoc = GetShaderLocation(shaderCoCNearMaxFilter, "horizontal_pass");
    horizontalPassBlurLoc = GetShaderLocation(shaderCoCNearBlur, "horizontal_pass");
    shaderCoCNearBlurTexLoc = GetShaderLocation(shaderCoCNearBlur, "coc_texture_max");
}

GatherBasedDoF::~GatherBasedDoF()
{
    UnloadRenderTexture(cocTex);
    UnloadMultiRenderTexture(downsamplePass);
}

void GatherBasedDoF::render(Lights* lights){
    lights->updateShaderValues(true);
    
    screenTexPass();
    cocTexPass();
    downSamplePass();


    //MAX FILTER
    BeginTextureMode(cocNearBlurred);
        ClearBackground(BLANK);
    EndTextureMode();

    //Horizontal max filter pass
    cocNearMaxFilterPass(true);
    //Vertical max filter pass
    cocNearMaxFilterPass(false);

    //BLUR OF MAX FILTER
    cocNearBlurPass(true);
    cocNearBlurPass(false);
    
    BeginDrawing();
        // ClearBackground(RAYWHITE);
        // DrawTextureRec(cocNearBlurred.texture, (Rectangle){ 0, 0, (float)cocTex.texture.width, (float)-cocTex.texture.height }, (Vector2){ 0, 0 }, WHITE);
        DrawTexturePro(cocNearBlurred.texture, (Rectangle){ 0, 0, (float)cocNearBlurred.texture.width, (float)-cocNearBlurred.texture.height },
                (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height },(Vector2){ 0, 0 }, 0,WHITE);

        rlImGuiBegin();	
            drawUI();
        rlImGuiEnd();
        DrawFPS(10, 10);
    EndDrawing();
}


void GatherBasedDoF::screenTexPass(){
    BeginTextureMode(Utils::sScreen_tex);
        rlDisableColorBlend();
        ClearBackground(Utils::sClearColor);
        Utils::draw_scene();
        rlEnableColorBlend();
    EndTextureMode();
}
void GatherBasedDoF::cocTexPass(){
    //CoC tex
    BeginTextureMode(cocTex);
        ClearBackground(RAYWHITE);
            rlDisableColorBlend();
            BeginShaderMode(shaderCoC),
                SetShaderValueTexture(shaderCoC,shaderCoCTexLoc,Utils::sScreen_tex.texture);
                SetShaderValue(shaderCoC,lensSettingsLoc,&Utils::lensParams, RL_SHADER_UNIFORM_VEC2);
                DrawTextureRec(Utils::sScreen_tex.texture, (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();
            rlEnableColorBlend();
    EndTextureMode();
}

void GatherBasedDoF::downSamplePass(){
    //Downsample, does not work without BeginTextureMode, so I just put a random texture with same size
    BeginTextureMode(DSTex);
            rlDisableColorBlend();
                rlEnableFramebuffer(downsamplePass.id);
                    
                    rlActiveDrawBuffers(3);
                    rlClearScreenBuffers();
                    
                    BeginShaderMode(shaderDS),
                        SetShaderValueTexture(shaderDS,shaderDsScreenLoc,Utils::sScreen_tex.texture);
                        SetShaderValueTexture(shaderDS,shaderDsCoCLoc,cocTex.texture);
                        DrawTextureRec(DSTex.texture, (Rectangle){ 0, 0, (float)DSTex.texture.width, (float)-DSTex.texture.height }, (Vector2){ 0, 0 }, WHITE);
                    EndShaderMode();

                rlDisableFramebuffer();
            rlEnableColorBlend();
    EndTextureMode();
}

void GatherBasedDoF::cocNearMaxFilterPass(bool horizontal){
    horizontalPass = horizontal;

    SetTextureFilter(cocNearBlurred.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureWrap(cocNearBlurred.texture, TEXTURE_WRAP_CLAMP);

    BeginTextureMode(cocNearBlurred);
        // ClearBackground(RAYWHITE);
        rlDisableColorBlend();

            BeginShaderMode(shaderCoCNearMaxFilter),
                SetShaderValue(shaderCoCNearMaxFilter,horizontalPassLoc,&horizontalPass,RL_SHADER_UNIFORM_INT);
                SetShaderValueTexture(shaderCoCNearMaxFilter,shaderCoCNearTexLoc,cocTex.texture);
                SetShaderValueTexture(shaderCoCNearMaxFilter,shaderCoCNearMaxTexLoc,cocNearBlurred.texture);
                DrawTextureRec(cocNearBlurred.texture, (Rectangle){ 0, 0, (float)cocNearBlurred.texture.width, (float)-cocNearBlurred.texture.height }, (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();
        
        rlEnableColorBlend();
    EndTextureMode();
}

void GatherBasedDoF::cocNearBlurPass(bool horizontal){

    horizontalPass = horizontal;

    BeginTextureMode(cocNearBlurred);
        // ClearBackground(RAYWHITE);
        rlDisableColorBlend();
            BeginShaderMode(shaderCoCNearBlur),
                SetShaderValue(shaderCoCNearBlur,horizontalPassBlurLoc,&horizontalPass,RL_SHADER_UNIFORM_INT);
                SetShaderValueTexture(shaderCoCNearBlur,shaderCoCNearBlurTexLoc,cocNearBlurred.texture);
                DrawTextureRec(cocNearBlurred.texture, (Rectangle){ 0, 0, (float)cocNearBlurred.texture.width, (float)-cocNearBlurred.texture.height }, (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();
        
        rlEnableColorBlend();
    EndTextureMode();
}


void GatherBasedDoF::drawUI(){
    ImGui::Begin("DoF settings");
    ImGui::SliderInt("Technique",&Utils::sTechnique, 0,3);
    ImGui::SliderFloat2("Focus distance ; Focus range",&Utils::lensParams.x, 0.0f,30.0f);
    ImGui::End();
}

//Code By Raysan5, creator of raylib

// Load multi render texture (framebuffer)
// NOTE: Render texture is loaded by default with RGBA color attachment and depth RenderBuffer
MultiRenderTexture GatherBasedDoF::LoadMultiRenderTexture(int width, int height)
{
    MultiRenderTexture target = { 0 };

    target.id = rlLoadFramebuffer();   // Load an empty framebuffer

    if (target.id > 0)
    {
        rlEnableFramebuffer(target.id);

        // Create color texture: color
        target.texColor.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
        target.texColor.width = width;
        target.texColor.height = height;
        target.texColor.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        target.texColor.mipmaps = 1;
        
        // Create color texture: normal
        target.texNormal.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
        target.texNormal.width = width;
        target.texNormal.height = height;
        target.texNormal.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        target.texNormal.mipmaps = 1;
        
        // Create color texture: position
        target.texPosition.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
        target.texPosition.width = width;
        target.texPosition.height = height;
        target.texPosition.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        target.texPosition.mipmaps = 1;

        // Create depth texture
        target.texDepth.id = rlLoadTextureDepth(width, height, false);
        target.texDepth.width = width;
        target.texDepth.height = height;
        target.texDepth.format = 19;       //DEPTH_COMPONENT_24BIT?
        target.texDepth.mipmaps = 1;

        // Attach color textures and depth textures to FBO
        rlFramebufferAttach(target.id, target.texColor.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(target.id, target.texNormal.id, RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(target.id, target.texPosition.id, RL_ATTACHMENT_COLOR_CHANNEL2, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(target.id, target.texDepth.id, RL_ATTACHMENT_DEPTH, RL_ATTACHMENT_RENDERBUFFER, 0);
        
        // Activate required color draw buffers
        rlActiveDrawBuffers(3);

        // Check if fbo is complete with attachments (valid)
        if (rlFramebufferComplete(target.id)) TRACELOG(LOG_INFO, "FBO: [ID %i] MultiRenderTexture loaded successfully", target.id);

        rlDisableFramebuffer();
    }
    else TRACELOG(LOG_WARNING, "FBO: MultiRenderTexture can not be created");

    return target;
}

// Unload multi render texture from GPU memory (VRAM)
void GatherBasedDoF::UnloadMultiRenderTexture(MultiRenderTexture target)
{
    if (target.id > 0)
    {
        // Delete color texture attachments
        rlUnloadTexture(target.texColor.id);
        rlUnloadTexture(target.texNormal.id);
        rlUnloadTexture(target.texPosition.id);

        // NOTE: Depth texture is automatically queried
        // and deleted before deleting framebuffer
        rlUnloadFramebuffer(target.id);
    }
}