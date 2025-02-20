#include "lights.h"

Lights::Lights(/* args */)
{
    lightShader = LoadShader("./src/shaders/lighting/lighting.vs","./src/shaders/lighting/lighting.fs");
    bypassLightShader = LoadShader("./src/shaders/lighting/lighting.vs","./src/shaders/lighting/lights.fs");
    sunlightPos = (Vector3){ 11, 11, 9 };
    lights[0] = CreateLight(LIGHT_DIRECTIONAL, sunlightPos, Vector3Zero(), {133,154,229}, lightShader);
    lights[0].positionLoc = GetShaderLocation(lightShader,"lights[0].position");
    // lights[0] = CreateLight(LIGHT_POINT, (Vector3){ -2, 1, -2 }, Vector3Zero(), YELLOW, lightShader);
    // lights[1] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, 2 }, Vector3Zero(), RED, lightShader);
    // lights[2] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, 18 }, Vector3Zero(), RED, lightShader);

    lightShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lightShader, "viewPos");
    ambientLoc = GetShaderLocation(lightShader, "ambient");
    lensSettingsLoc = GetShaderLocation(lightShader, "lens_settings");
    setDepthLoc = GetShaderLocation(lightShader, "set_depth");
    setDepthLightsLoc = GetShaderLocation(bypassLightShader, "set_depth");
    setDepth = 1;

}

Lights::~Lights()
{
}

void Lights::updateShaderValues(int setDepth){
    BeginShaderMode(lightShader);

        SetShaderValue(lightShader, lightShader.locs[SHADER_LOC_VECTOR_VIEW], &Utils::camera.position, SHADER_UNIFORM_VEC3);
        lights[0].position = sunlightPos;

        for (int i = 0; i < MAX_LIGHTS; i++) 
            UpdateLightValues(lightShader, lights[i]);
        
        SetShaderValue(lightShader, ambientLoc, (float[4]){ 0.1f, 0.1f, 0.1f, 1.0f }, SHADER_UNIFORM_VEC4);
        SetShaderValue(lightShader, ambientLoc, (float[4]){ 0.1f, 0.1f, 0.1f, 1.0f }, SHADER_UNIFORM_VEC4);
        SetShaderValue(lightShader, ambientLoc, (float[4]){ 0.1f, 0.1f, 0.1f, 1.0f }, SHADER_UNIFORM_VEC4);
        SetShaderValue(lightShader, setDepthLoc, &setDepth, SHADER_UNIFORM_INT);
    EndShaderMode();
    
    BeginShaderMode(bypassLightShader);
        SetShaderValue(bypassLightShader, setDepthLightsLoc, &setDepth, SHADER_UNIFORM_INT);
    EndShaderMode();
}

