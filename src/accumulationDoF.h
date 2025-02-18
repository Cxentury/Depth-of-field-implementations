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
    float offsetFactor = 0.257f;
    int randomSampling = 0;
    Vector3 focusTarget = {0};

    Shader accumulationShader = LoadShader(0,"./src/shaders/accumulation/accumulation.fs");
    Shader passThroughShader = LoadShader(0,"./src/shaders/accumulation/pass_through.fs");

    int accumulatedTexLoc = GetShaderLocation(accumulationShader,"accumulated_texture");
    int nbSamplesLoc = GetShaderLocation(accumulationShader,"samples");
    int passThroughTextureLoc = GetShaderLocation(passThroughShader,"texture0");
    
    Vector3 samples[500];

    AccumulationDoF(/* args */);
    ~AccumulationDoF();

    void generateSamples();
    void render(Lights* lights);
    void drawUI(Vector3* sunlightPos);
};
#endif