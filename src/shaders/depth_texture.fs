#version 330 core

void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z);
    finalColor = vec4(fragColor.rgb,depth);
} 