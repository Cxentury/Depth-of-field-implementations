#ifndef GATHER_BASED
#define GATHER_BASED

#include "utils.h"
#include "lights.h"

//Code By Raysan5, creator of raylib

// MultiRenderTexture, aka GBuffers/MRT
// NOTE: This advance render target let us draw to multiple texture color buffers at one,
// very useful for deferred shading and other techniques (store specularity, ambient occlusion data...)
struct MultiRenderTexture {
    unsigned int id;        // OpenGL framebuffer object id
    int width;              // Color buffers width (same all buffers)
    int height;             // Color buffers height (same all buffers)
    Texture texColor;       // Color buffer attachment: color data
    Texture texNormal;      // Color buffer attachment: normal data
    Texture texPosition;    // Color buffer attachment: position data
    Texture texDepth;       // Depth buffer attachment
};

class GatherBasedDoF
{

    private:


        void UnloadMultiRenderTexture(MultiRenderTexture target);
        MultiRenderTexture LoadMultiRenderTexture(int width, int height);

        //---------------

        RenderTexture2D cocTex;
        RenderTexture2D DSTex;
        RenderTexture2D cocNearBlurred;
        MultiRenderTexture downsamplePass;
        
        Shader shaderCoC;
        Shader shaderDS;
        Shader shaderCoCNearBlur;

        
        int shaderCoCTexLoc;
        int lensSettingsLoc;

        //DS = downsample;
        int shaderDsScreenLoc;
        int shaderDsCoCLoc;

        //huge
        int shaderCoCNearBlurTexLoc;
        
    public:
        GatherBasedDoF(/* args */);
        ~GatherBasedDoF();
        void render(Lights* lights);
        void drawUI();
        void screenTexPass();
        void cocTexPass();
        void downSamplePass();
        void cocNearPass();
};

#endif