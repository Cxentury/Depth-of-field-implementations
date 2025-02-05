#version 330 core

in vec2 fragTexCoord;

out vec4 finalColor;

uniform sampler2D screen_texture;
uniform vec2 lens_settings;

void main()
{
    //CoC
    float focus_distance = lens_settings.x;
    float focus_range = lens_settings.y;

    float depth = texture(screen_texture,fragTexCoord).a;
    float coc = (depth - focus_distance) / focus_range;
	coc = clamp(coc, -1.0,1.0);
    
    finalColor.rgb = vec3(0);

    if(coc < 0)
        finalColor.r = abs(coc);
    else
        finalColor.g = coc;
}