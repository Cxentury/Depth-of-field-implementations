#version 330 core

in vec2 fragTexCoord;

uniform sampler2D screen_texture;

void main()
{
    vec2 pixelSize = 1.0 / textureSize(screen_texture, 0);

    finalColor = textureLod(screen_texture, fragTexCoord,0);
}