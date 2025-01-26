#ifndef ACCUMULATION_DOF
#define ACCUMULATION_DOF

#include "utils.h"
#include "lights.h"

class AccumulationDoF
{
private:
    /* data */
public:
    int sampleCount = 256;
    float offsetFactor = 0.001;
    Shader accumulationShader = LoadShader(0,"./src/shaders/accumulation.fs");
    int accumulatedTexLoc = GetShaderLocation(accumulationShader,"accumulated_texture");
    int nbSamplesLoc = GetShaderLocation(accumulationShader,"samples");
    Vector3 samples[3000];
    AccumulationDoF(/* args */);
    ~AccumulationDoF();

    void render(Lights* lights);
    void drawUI();
    Vector4 fadeColor(Color color, float alpha);
    void modified_DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Vector4 tint);
    void modified_DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Vector4 tint);
};
#endif