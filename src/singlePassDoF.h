#ifndef SINGLE_PASS_DOF
#define SINGLE_PASS_DOF

#include <raylib.h>
#include "imgui.h"
#include "utils.h"
#include "lights.h"

class SinglePassDoF
{
    private:
        /* data */
        Shader SinglePassDoFShader;
        int screenTexLoc;
        int farPlaneLoc;
        int lensSettingsLoc;

    public:
        SinglePassDoF(/* args */);
        ~SinglePassDoF();
        void drawUI(Vector3* sunlightPos);
        void render(Lights* lights);
        void shaderDoF();
};

#endif