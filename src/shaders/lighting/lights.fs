#version 330 core

in vec4 fragColor;

out vec4 finalColor;

uniform int set_depth;

float near = 0.1;
float far  = 1000.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}


void main()
{
    finalColor.rgb = fragColor.rgb;
    finalColor.a = 1.0;

    if(set_depth == 1){
        float depth = LinearizeDepth(gl_FragCoord.z);
        finalColor.a = depth;
    }
}