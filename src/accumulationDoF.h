#ifndef ACCUMULATION_DOF
#define ACCUMULATION_DOF

#include "utils.h"
#include "lights.h"

class AccumulationDoF
{
private:
    /* data */
public:
    int sampleCount = 400;
    float offsetFactor = 0.2;
    Shader accumulationShader = LoadShader(0,"./src/shaders/accumulation.fs");
    int accumulatedTexLoc = GetShaderLocation(accumulationShader,"accumulated_texture");
    int nbSamplesLoc = GetShaderLocation(accumulationShader,"samples");
    Vector3 samples[3000];
    int randomSampling = 0;

    AccumulationDoF(/* args */);
    ~AccumulationDoF();

    void render(Lights* lights);
    void drawUI(Vector3* sunlightPos);
};
#endif