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
    lights->updateShaderValues();
    float alphaFac = 255.0/(float)sampleCount;


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
            Utils::draw_scene();
            rlEnableColorBlend();
        EndTextureMode();

        BeginTextureMode(Utils::sCoC_tex);
            rlSetBlendFactors(GL_CONSTANT_ALPHA,GL_ONE,GL_FUNC_SUBTRACT);
            rlEnableColorBlend();
            BeginBlendMode(RL_BLEND_CUSTOM);
            glBlendColor(0,0,0,1.0/alphaFac);
            DrawTextureRec(Utils::sScreen_tex.texture, (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height, }, (Vector2){ 0, 0 },WHITE);
            // modified_DrawTextureRec(Utils::sScreen_tex.texture, (Rectangle){ 0, 0, (float)Utils::sScreen_tex.texture.width, (float)-Utils::sScreen_tex.texture.height, }, (Vector2){ 0, 0 }, fadeColor(WHITE,alphaFac));
            EndBlendMode();
            // rlSetBlendMode(RL_BLEND_ALPHA);
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
    ImGui::SliderInt("Number of Samples",&sampleCount, 0,1000);
    ImGui::SliderFloat("Offset factor",&offsetFactor, 0.001f,1.0f);
    ImGui::End();        
}


Vector4 AccumulationDoF::fadeColor(Color color, float alpha){
    Vector4 result = {color.r,color.g,color.b,0};

    if (alpha < 0.0f) alpha = 0.0f;
    else if (alpha > 1.0f) alpha = 1.0f;

    result.w = 250.0*alpha;

    return result;
}

void AccumulationDoF::modified_DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Vector4 tint){
    Rectangle dest = { position.x, position.y, fabsf(source.width), fabsf(source.height) };
    Vector2 origin = { 0.0f, 0.0f };

    modified_DrawTexturePro(texture, source, dest, origin, 0.0f, tint);
}

void AccumulationDoF::modified_DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Vector4 tint){
 // Check if texture is valid
    if (texture.id > 0)
    {
        float width = (float)texture.width;
        float height = (float)texture.height;

        bool flipX = false;

        if (source.width < 0) { flipX = true; source.width *= -1; }
        if (source.height < 0) source.y -= source.height;

        if (dest.width < 0) dest.width *= -1;
        if (dest.height < 0) dest.height *= -1;

        Vector2 topLeft = { 0 };
        Vector2 topRight = { 0 };
        Vector2 bottomLeft = { 0 };
        Vector2 bottomRight = { 0 };

        // Only calculate rotation if needed
        if (rotation == 0.0f)
        {
            float x = dest.x - origin.x;
            float y = dest.y - origin.y;
            topLeft = (Vector2){ x, y };
            topRight = (Vector2){ x + dest.width, y };
            bottomLeft = (Vector2){ x, y + dest.height };
            bottomRight = (Vector2){ x + dest.width, y + dest.height };
        }
        else
        {
            float sinRotation = sinf(rotation*DEG2RAD);
            float cosRotation = cosf(rotation*DEG2RAD);
            float x = dest.x;
            float y = dest.y;
            float dx = -origin.x;
            float dy = -origin.y;

            topLeft.x = x + dx*cosRotation - dy*sinRotation;
            topLeft.y = y + dx*sinRotation + dy*cosRotation;

            topRight.x = x + (dx + dest.width)*cosRotation - dy*sinRotation;
            topRight.y = y + (dx + dest.width)*sinRotation + dy*cosRotation;

            bottomLeft.x = x + dx*cosRotation - (dy + dest.height)*sinRotation;
            bottomLeft.y = y + dx*sinRotation + (dy + dest.height)*cosRotation;

            bottomRight.x = x + (dx + dest.width)*cosRotation - (dy + dest.height)*sinRotation;
            bottomRight.y = y + (dx + dest.width)*sinRotation + (dy + dest.height)*cosRotation;
        }

        rlSetTexture(texture.id);
        rlBegin(RL_QUADS);

            rlColor4f(tint.x, tint.y, tint.z, tint.w);
            rlNormal3f(0.0f, 0.0f, 1.0f);                          // Normal vector pointing towards viewer

            // Top-left corner for texture and quad
            if (flipX) rlTexCoord2f((source.x + source.width)/width, source.y/height);
            else rlTexCoord2f(source.x/width, source.y/height);
            rlVertex2f(topLeft.x, topLeft.y);

            // Bottom-left corner for texture and quad
            if (flipX) rlTexCoord2f((source.x + source.width)/width, (source.y + source.height)/height);
            else rlTexCoord2f(source.x/width, (source.y + source.height)/height);
            rlVertex2f(bottomLeft.x, bottomLeft.y);

            // Bottom-right corner for texture and quad
            if (flipX) rlTexCoord2f(source.x/width, (source.y + source.height)/height);
            else rlTexCoord2f((source.x + source.width)/width, (source.y + source.height)/height);
            rlVertex2f(bottomRight.x, bottomRight.y);

            // Top-right corner for texture and quad
            if (flipX) rlTexCoord2f(source.x/width, source.y/height);
            else rlTexCoord2f((source.x + source.width)/width, source.y/height);
            rlVertex2f(topRight.x, topRight.y);

        rlEnd();
        rlSetTexture(0);
    }
}