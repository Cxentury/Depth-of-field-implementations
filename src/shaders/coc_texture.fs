#version 330 core

// focus distance ; focus range
uniform vec2 lens_settings;

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
    float focus_distance = lens_settings.x;
    float focus_range = lens_settings.y;

    float depth = LinearizeDepth(gl_FragCoord.z);
    float coc = (depth - focus_distance) / focus_range;
	coc = clamp(coc, -1.0,1.0);
    // finalColor = vec4(vec3(coc),1.0);
    finalColor = vec4(fragColor.rgb,coc);
} 