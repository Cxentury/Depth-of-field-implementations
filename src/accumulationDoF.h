#ifndef ACCUMULATION_DOF
#define ACCUMULATION_DOF

#include "utils.h"
#include "lights.h"

class AccumulationDoF
{
private:
    /* data */
public:
    int sampleCount = 250;
    float offsetFactor = 0.6;
    Shader accumulationShader = LoadShader(0,"./src/shaders/accumulation.fs");
    int accumulatedTexLoc = GetShaderLocation(accumulationShader,"accumulated_texture");
    int nbSamplesLoc = GetShaderLocation(accumulationShader,"samples");
    Vector3 samples[3000];
    AccumulationDoF(/* args */);
    ~AccumulationDoF();

    void render(Lights* lights);
    void drawUI();
};
#endif