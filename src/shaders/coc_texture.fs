#version 330 core

uniform float focus_distance;
uniform float focus_range;

in vec4 fragColor;
out vec4 finalColor;      

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


void main()
{
    float depth = LinearizeDepth(gl_FragCoord.z);
    float coc = (depth - focus_distance) / focus_range;
	coc = clamp(coc, -1.0,1.0);
    // finalColor = vec4(vec3(coc),1.0);
    // finalColor = vec4(fragColor.rgb,1.0);
    finalColor = vec4(fragColor.rgb,coc);
} 