#include "lights.h"

Lights::Lights(/* args */)
{
    lightShader = LoadShader("./src/shaders/lighting/lighting.vs","./src/shaders/lighting/lighting.fs");
    lights[0] = CreateLight(LIGHT_POINT, (Vector3){ -2, 1, -2 }, Vector3Zero(), YELLOW, lightShader);
    lights[1] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, 2 }, Vector3Zero(), RED, lightShader);
    // lights[2] = CreateLight(LIGHT_POINT, (Vector3){ -2, 1, 2 }, Vector3Zero(), GREEN, lightShader);
    lights[2] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, 18 }, Vector3Zero(), RED, lightShader);

    lightShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lightShader, "viewPos");
    ambientLoc = GetShaderLocation(lightShader, "ambient");
    lensSettingsLoc = GetShaderLocation(lightShader, "lens_settings");
}

Lights::~Lights()
{
}

void Lights::updateShaderValues(){
    for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(lightShader, lights[i]);
    SetShaderValue(lightShader, ambientLoc, (float[4]){ 0.1f, 0.1f, 0.1f, 1.0f }, SHADER_UNIFORM_VEC4);
    SetShaderValue(lightShader, ambientLoc, (float[4]){ 0.1f, 0.1f, 0.1f, 1.0f }, SHADER_UNIFORM_VEC4);
    // for (int i = 0; i < MAX_LIGHTS; i++)
    // {
    //     if (lights[i].enabled) DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
    //     else DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
    // }
}

