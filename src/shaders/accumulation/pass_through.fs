#version 330 core

in vec2 fragTexCoord;

uniform sampler2D texture0;

out vec4 finalColor;

void main()
{
    finalColor = texture(texture0,fragTexCoord);
} 