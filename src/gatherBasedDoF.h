#ifndef GATHER_BASED
#define GATHER_BASED

#include "utils.h"
#include "lights.h"

//Code for MultiRenderTexture By Raysan5, creator of raylib

// MultiRenderTexture, aka GBuffers/MRT
// NOTE: This advance render target let us draw to multiple texture color buffers at one,
// very useful for deferred shading and other techniques (store specularity, ambient occlusion data...)
struct MultiRenderTexture {
    unsigned int id;        // OpenGL framebuffer object id
    int width;              // Color buffers width (same all buffers)
    int height;             // Color buffers height (same all buffers)
    Texture texture0;       // Color buffer attachment: color data
    Texture texture1;      // Color buffer attachment: normal data
    Texture texture2;    // Color buffer attachment: position data
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
        RenderTexture2D cocNearBlurredTex;
        RenderTexture2D prevCompositeTex;

        MultiRenderTexture downsamplePassTex;
        MultiRenderTexture computationPassTex;
        MultiRenderTexture fillPassTex;
        
        Shader shaderCoC;
        Shader shaderDS;
        Shader shaderCoCNearMaxFilter;
        Shader shaderCoCNearBlur;
        Shader shaderComputation;
        Shader shaderFill;
        Shader shaderComposite;
        Shader shaderComposite2;
        
        int shaderCoCTexLoc;
        int lensSettingsLoc;
        //DS = downsample;
        int shaderDsScreenLoc;
        int shaderDsCoCLoc;

        bool horizontalPass = true;
        int horizontalPassLoc;

        int shaderCoCNearTexLoc;
        //huge
        int shaderCoCNearMaxTexLoc;
        
        int horizontalPassBlurLoc;
        int shaderCoCNearBlurTexLoc;
        
        float dofStrength = .46f;
        float kernelScale = 1.0f;
        float blend = 1.0;
        int kernelScaleLoc;
        int shaderCompositeBlendLoc, shaderComposite2BlendLoc;

        int shaderComputationTexturesLoc[4];
        
        int shaderFillTexturesLoc[4];
        
        int shaderCompositeTexturesLoc[6];
        int shaderComposite2TexturesLoc[3];

    public:
        GatherBasedDoF(/* args */);
        ~GatherBasedDoF();

        void render(Lights* lights);
        void drawUI();

        void loadTextures();
        void unloadTextures();
        void screenTexPass();
        void cocTexPass();
        void downSamplePass();
        void cocNearMaxFilterPass(bool horizontal = true);
        void cocNearBlurPass(bool horizontal = true);
        void computationPass();
        void fillPass();
        void compositePass();

        void SetShaderValueTextureNoLimit(Shader shader, int locIndex, Texture2D texture);
        void rlSetUniformSamplerNoLimit(int locIndex, unsigned int textureId);
};

#endif