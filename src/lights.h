#ifndef DOF_LIGHTS_H
#define DOF_LIGHTS_H

#include <raylib.h>

#include "raymath.h"
#include "rlImGui.h"
#include "imgui.h"
#include "rlights.h"

class Lights{
    private:
        int ambientLoc;

    public:
        
        Light lights[MAX_LIGHTS];
        Shader lightShader;
        int lensSettingsLoc;
        int setDepth;
        int setDepthLoc;
        

        Lights(/* args */);
        ~Lights();

        void updateShaderValues(int setDepth);
        void drawUI();
};
#endif