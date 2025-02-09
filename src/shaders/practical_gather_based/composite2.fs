#version 430 core

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform float blend;
uniform sampler2D prev_composite;
uniform sampler2D coc_near_blurred;
uniform sampler2D near_field;

void main(){

    float cocNear = textureLod(coc_near_blurred, fragTexCoord, 0).x;
    vec4 dofNear = textureLod(near_field, fragTexCoord, 0);
    
    vec4 result = textureLod(prev_composite,fragTexCoord,0);
    result = mix(result, dofNear, cocNear * blend);
    finalColor = result;
}