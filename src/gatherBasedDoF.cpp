#include "GatherBasedDoF.h"
#include "glad.h"

GatherBasedDoF::GatherBasedDoF(/* args */)
{   
    loadTextures();

    shaderCoC = LoadShader(0, TextFormat("./src/shaders/practical_gather_based/coc.fs", GLSL_VERSION));
    shaderDS = LoadShader(0, TextFormat("./src/shaders/practical_gather_based/downsample.fs", GLSL_VERSION));
    shaderCoCNearMaxFilter = LoadShader(0, TextFormat("./src/shaders/practical_gather_based/nearCoCMaxFilter.fs", GLSL_VERSION));
    shaderCoCNearBlur = LoadShader(0, TextFormat("./src/shaders/practical_gather_based/nearCoCBlur.fs", GLSL_VERSION));
    shaderComputation = LoadShader(0, TextFormat("./src/shaders/practical_gather_based/computation.fs", GLSL_VERSION));
    shaderFill = LoadShader(0, TextFormat("./src/shaders/practical_gather_based/fill.fs", GLSL_VERSION));
    shaderComposite = LoadShader(0, TextFormat("./src/shaders/practical_gather_based/composite.fs", GLSL_VERSION));
    shaderComposite2 = LoadShader(0, TextFormat("./src/shaders/practical_gather_based/composite2.fs", GLSL_VERSION));
    
    shaderCoCTexLoc = GetShaderLocation(shaderCoC,"screen_texture");
    lensSettingsLoc = GetShaderLocation(shaderCoC, "lens_settings");
    
    shaderDsScreenLoc = GetShaderLocation(shaderDS, "screen_texture");
    shaderDsCoCLoc = GetShaderLocation(shaderDS, "coc_texture");

    shaderCoCNearTexLoc = GetShaderLocation(shaderCoCNearMaxFilter, "coc_texture");
    shaderCoCNearMaxTexLoc = GetShaderLocation(shaderCoCNearMaxFilter, "coc_texture_max");
    horizontalPassLoc = GetShaderLocation(shaderCoCNearMaxFilter, "horizontal_pass");
    horizontalPassBlurLoc = GetShaderLocation(shaderCoCNearBlur, "horizontal_pass");
    shaderCoCNearBlurTexLoc = GetShaderLocation(shaderCoCNearBlur, "coc_texture_max");

    kernelScaleLoc = GetShaderLocation(shaderComputation, "kernel_scale");
    shaderComputationTexturesLoc[0] = GetShaderLocation(shaderComputation, "downsampled_color");
    shaderComputationTexturesLoc[1] = GetShaderLocation(shaderComputation, "downsampled_coc");
    shaderComputationTexturesLoc[2] = GetShaderLocation(shaderComputation, "downsampled_coc_mul_far");
    shaderComputationTexturesLoc[3] = GetShaderLocation(shaderComputation, "coc_near_blurred");

    shaderCompositeTexturesLoc[0] = GetShaderLocation(shaderComposite, "screen_texture");
    shaderCompositeTexturesLoc[1] = GetShaderLocation(shaderComposite, "coc_texture");
    shaderCompositeTexturesLoc[2] = GetShaderLocation(shaderComposite, "downsampled_coc");
    shaderCompositeTexturesLoc[3] = GetShaderLocation(shaderComposite, "coc_near_blurred");
    shaderCompositeTexturesLoc[4] = GetShaderLocation(shaderComposite, "near_field");
    shaderCompositeTexturesLoc[5] = GetShaderLocation(shaderComposite, "far_field");
    shaderCompositeBlendLoc = GetShaderLocation(shaderComposite, "blend");
    
    shaderComposite2TexturesLoc[0] = GetShaderLocation(shaderComposite2,"prev_composite");
    shaderComposite2TexturesLoc[1] = GetShaderLocation(shaderComposite2,"coc_near_blurred");
    shaderComposite2TexturesLoc[2] = GetShaderLocation(shaderComposite2,"near_field");
    shaderComposite2BlendLoc = GetShaderLocation(shaderComposite2, "blend");

    shaderFillTexturesLoc[0] = GetShaderLocation(shaderFill, "downsampled_coc");
    shaderFillTexturesLoc[1] = GetShaderLocation(shaderFill, "coc_near_blurred");
    shaderFillTexturesLoc[2] = GetShaderLocation(shaderFill, "dof_near");
    shaderFillTexturesLoc[3] = GetShaderLocation(shaderFill, "dof_far");
}

GatherBasedDoF::~GatherBasedDoF()
{
}

void GatherBasedDoF::loadTextures(){
    
    cocTex = Utils::LoadRenderTextureRGB8(Utils::sScreen_width, Utils::sScreen_height);
    DSTex = Utils::LoadRenderTextureRGB8(Utils::sScreen_width/2, Utils::sScreen_height/2);
    cocNearBlurredTex = Utils::LoadRenderTextureRGB8(Utils::sScreen_width/2, Utils::sScreen_height/2);
    
    downsamplePassTex = LoadMultiRenderTexture(Utils::sScreen_width/2, Utils::sScreen_height/2);
    computationPassTex = LoadMultiRenderTexture(Utils::sScreen_width/2, Utils::sScreen_height/2);
    fillPassTex = LoadMultiRenderTexture(Utils::sScreen_width/2, Utils::sScreen_height/2);
    prevCompositeTex = Utils::LoadRenderTextureRGBA16(Utils::sScreen_width, Utils::sScreen_height);
}

void GatherBasedDoF::unloadTextures(){
    UnloadRenderTexture(cocTex);
    UnloadRenderTexture(DSTex);
    UnloadRenderTexture(cocNearBlurredTex);
    UnloadRenderTexture(prevCompositeTex);
    
    UnloadMultiRenderTexture(downsamplePassTex);
    UnloadMultiRenderTexture(computationPassTex);
    UnloadMultiRenderTexture(fillPassTex);
}


void GatherBasedDoF::render(Lights* lights){
    lights->updateShaderValues(true);
    
    screenTexPass();
    cocTexPass();
    downSamplePass();


    //MAX FILTER
    BeginTextureMode(cocNearBlurredTex);
        ClearBackground(BLANK);
    EndTextureMode();

    //Horizontal max filter pass
    cocNearMaxFilterPass(true);
    //Vertical max filter pass
    cocNearMaxFilterPass(false);

    //BLUR OF MAX FILTER
    cocNearBlurPass(true);
    cocNearBlurPass(false);
    
    computationPass();
    fillPass();

    BeginDrawing();
        // ClearBackground(RAYWHITE);
        // DrawTextureRec(cocNearBlurredTex.texture, (Rectangle){ 0, 0, (float)cocTex.texture.width, (float)-cocTex.texture.height }, (Vector2){ 0, 0 }, WHITE);
        compositePass();

        // DrawTexturePro(cocNearBlurredTex.texture, (Rectangle){ 0, 0, (float)cocNearBlurredTex.texture.width, (float)-cocNearBlurredTex.texture.height },
        //         (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height },(Vector2){ 0, 0 }, 0,WHITE);

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
                rlEnableFramebuffer(downsamplePassTex.id);
                    
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

    SetTextureFilter(cocNearBlurredTex.texture, TEXTURE_FILTER_BILINEAR);
    SetTextureWrap(cocNearBlurredTex.texture, TEXTURE_WRAP_CLAMP);

    BeginTextureMode(cocNearBlurredTex);
        // ClearBackground(RAYWHITE);
        rlDisableColorBlend();

            BeginShaderMode(shaderCoCNearMaxFilter),
                SetShaderValue(shaderCoCNearMaxFilter,horizontalPassLoc,&horizontalPass,RL_SHADER_UNIFORM_INT);
                SetShaderValueTexture(shaderCoCNearMaxFilter,shaderCoCNearTexLoc,cocTex.texture);
                SetShaderValueTexture(shaderCoCNearMaxFilter,shaderCoCNearMaxTexLoc,cocNearBlurredTex.texture);
                DrawTextureRec(cocNearBlurredTex.texture, (Rectangle){ 0, 0, (float)cocNearBlurredTex.texture.width, (float)-cocNearBlurredTex.texture.height }, (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();
        
        rlEnableColorBlend();
    EndTextureMode();
}

void GatherBasedDoF::cocNearBlurPass(bool horizontal){

    horizontalPass = horizontal;

    BeginTextureMode(cocNearBlurredTex);
        // ClearBackground(RAYWHITE);
        rlDisableColorBlend();
            BeginShaderMode(shaderCoCNearBlur),
                SetShaderValue(shaderCoCNearBlur,horizontalPassBlurLoc,&horizontalPass,RL_SHADER_UNIFORM_INT);
                SetShaderValueTexture(shaderCoCNearBlur,shaderCoCNearBlurTexLoc,cocNearBlurredTex.texture);
                DrawTextureRec(cocNearBlurredTex.texture, (Rectangle){ 0, 0, (float)cocNearBlurredTex.texture.width, (float)-cocNearBlurredTex.texture.height }, (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();
        
        rlEnableColorBlend();
    EndTextureMode();
}

void GatherBasedDoF::computationPass(){
    BeginTextureMode(DSTex);
            ClearBackground(RAYWHITE);
            rlDisableColorBlend();
                rlEnableFramebuffer(computationPassTex.id);
                    
                    rlActiveDrawBuffers(2);
                    rlClearScreenBuffers();
                    
                    BeginShaderMode(shaderComputation),
                        SetShaderValue(shaderComputation,kernelScaleLoc,&kernelScale, RL_SHADER_UNIFORM_FLOAT);
                        SetShaderValueTexture(shaderComputation,shaderComputationTexturesLoc[0], downsamplePassTex.texture0);
                        SetShaderValueTexture(shaderComputation,shaderComputationTexturesLoc[1], downsamplePassTex.texture1);
                        SetShaderValueTexture(shaderComputation,shaderComputationTexturesLoc[2], downsamplePassTex.texture2);
                        SetShaderValueTexture(shaderComputation,shaderComputationTexturesLoc[3], cocNearBlurredTex.texture);

                        DrawTextureRec(DSTex.texture, (Rectangle){ 0, 0, (float)DSTex.texture.width, (float)-DSTex.texture.height }, (Vector2){ 0, 0 }, WHITE);
                    EndShaderMode();

                rlDisableFramebuffer();
            rlEnableColorBlend();
    EndTextureMode();
}

void GatherBasedDoF::fillPass(){
    BeginTextureMode(DSTex);
        ClearBackground(RAYWHITE);
        rlDisableColorBlend();
            rlEnableFramebuffer(fillPassTex.id);
                    
                rlActiveDrawBuffers(2);
                rlClearScreenBuffers();

                BeginShaderMode(shaderFill);
                    //Raylib limits the number of maximum active textures to 4, why..
                    SetShaderValueTexture(shaderFill,shaderFillTexturesLoc[0],downsamplePassTex.texture1);
                    SetShaderValueTexture(shaderFill,shaderFillTexturesLoc[1],cocNearBlurredTex.texture);
                    SetShaderValueTexture(shaderFill,shaderFillTexturesLoc[2],computationPassTex.texture0);
                    SetShaderValueTexture(shaderFill,shaderFillTexturesLoc[3],computationPassTex.texture1);
                    
                    DrawTextureRec(DSTex.texture, (Rectangle){ 0, 0, (float)DSTex.texture.width, (float)-DSTex.texture.height }, (Vector2){ 0, 0 }, WHITE);
                EndShaderMode();

            rlDisableFramebuffer();
        rlEnableColorBlend();
    EndTextureMode();

}

void GatherBasedDoF::compositePass(){
    BeginTextureMode(prevCompositeTex);
        ClearBackground(RAYWHITE);
        rlDisableColorBlend();
            BeginShaderMode(shaderComposite);
                //Raylib limits the number of maximum active textures to 4, ...
                SetShaderValue(shaderComposite,shaderCompositeBlendLoc,&blend,RL_SHADER_UNIFORM_FLOAT);
                SetShaderValueTexture(shaderComposite,shaderCompositeTexturesLoc[0],Utils::sScreen_tex.texture);
                SetShaderValueTexture(shaderComposite,shaderCompositeTexturesLoc[1],cocTex.texture);
                SetShaderValueTexture(shaderComposite,shaderCompositeTexturesLoc[2],downsamplePassTex.texture1);
                // SetShaderValueTexture(shaderComposite,shaderCompositeTexturesLoc[3],cocNearBlurredTex.texture);
                // SetShaderValueTexture(shaderComposite,shaderCompositeTexturesLoc[4],computationPassTex.texture0);
                SetShaderValueTexture(shaderComposite,shaderCompositeTexturesLoc[5],fillPassTex.texture1);
                DrawTextureRec(Utils::sScreen_tex.texture, (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
            EndShaderMode();
        rlEnableColorBlend();
    EndTextureMode();

    ClearBackground(RAYWHITE);
    rlDisableColorBlend();
        BeginShaderMode(shaderComposite2);
            SetShaderValue(shaderComposite2,shaderComposite2BlendLoc,&blend,RL_SHADER_UNIFORM_FLOAT);
            SetShaderValueTexture(shaderComposite2,shaderComposite2TexturesLoc[0],prevCompositeTex.texture);
            SetShaderValueTexture(shaderComposite2,shaderComposite2TexturesLoc[1],cocNearBlurredTex.texture);
            SetShaderValueTexture(shaderComposite2,shaderComposite2TexturesLoc[2],fillPassTex.texture0);
            DrawTextureRec(Utils::sScreen_tex.texture, (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height }, (Vector2){ 0, 0 }, WHITE);
        EndShaderMode();
    rlEnableColorBlend();
}

void GatherBasedDoF::drawUI(){

    if(dofStrength >= .25f){
        kernelScale = dofStrength;
        blend = 1.0f;
    }
    else{
        kernelScale = .25f;
        blend = 4.0f * dofStrength;
    }

    ImGui::Begin("DoF settings");
    ImGui::SliderInt("Technique",&Utils::sTechnique, 0,3);
    ImGui::SliderFloat("Strength",&dofStrength, 0,3);
    // ImGui::SliderFloat("kernel scale",&kernelScale, .1,2);
    // ImGui::SliderFloat("Blend",&blend, .1,3);
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
        target.texture0.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
        target.texture0.width = width;
        target.texture0.height = height;
        target.texture0.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        target.texture0.mipmaps = 1;
        
        // Create color texture: normal
        target.texture1.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R16G16B16, 1);
        target.texture1.width = width;
        target.texture1.height = height;
        target.texture1.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        target.texture1.mipmaps = 1;
        
        // Create color texture: position
        target.texture2.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
        target.texture2.width = width;
        target.texture2.height = height;
        target.texture2.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
        target.texture2.mipmaps = 1;

        // Create depth texture
        target.texDepth.id = rlLoadTextureDepth(width, height, false);
        target.texDepth.width = width;
        target.texDepth.height = height;
        target.texDepth.format = 19;       //DEPTH_COMPONENT_24BIT?
        target.texDepth.mipmaps = 1;

        // Attach color textures and depth textures to FBO
        rlFramebufferAttach(target.id, target.texture0.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(target.id, target.texture1.id, RL_ATTACHMENT_COLOR_CHANNEL1, RL_ATTACHMENT_TEXTURE2D, 0);
        rlFramebufferAttach(target.id, target.texture2.id, RL_ATTACHMENT_COLOR_CHANNEL2, RL_ATTACHMENT_TEXTURE2D, 0);
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
        rlUnloadTexture(target.texture2.id);
        rlUnloadTexture(target.texture1.id);
        rlUnloadTexture(target.texture2.id);

        // NOTE: Depth texture is automatically queried
        // and deleted before deleting framebuffer
        rlUnloadFramebuffer(target.id);
    }
}
