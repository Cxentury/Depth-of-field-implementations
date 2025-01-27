#version 330 core

in vec2 fragTexCoord;

uniform int samples;
uniform sampler2D accumulated_texture;

out vec4 finalColor;

void main()
{
    vec4 color = texture(accumulated_texture,fragTexCoord);
    color/=float(samples);
    finalColor = vec4(color);
} 