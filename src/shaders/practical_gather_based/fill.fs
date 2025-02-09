#version 330 core

layout(location = 0) out vec4 near_field;
layout(location = 1) out vec4 far_field;

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform float kernel_scale;

uniform sampler2D downsampled_coc;
uniform sampler2D coc_near_blurred;
uniform sampler2D dof_near;
uniform sampler2D dof_far;

void main(){

    vec2 pixelSize = 1.0 / textureSize(downsampled_coc, 0);

	float cocNearBlurrred = textureLod(coc_near_blurred, fragTexCoord, 0).r;

    if(cocNearBlurrred > 0.0){
        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                vec2 sampleCoords = fragTexCoord + vec2(i,j) * pixelSize;
                vec4 sample = textureLod(dof_near, sampleCoords, 0);
                near_field = max(near_field, sample);
            }            
        }
    }

    if(texture(downsampled_coc,fragTexCoord).y > 0.0){

        for(int i = -1; i <= 1; i++){
            for(int j = -1; j <= 1; j++){
                vec2 sampleCoords = fragTexCoord + vec2(i,j) * pixelSize;
                vec4 sample = textureLod(dof_far, sampleCoords, 0);
                far_field = max(far_field, sample);
            }            
        }
    }
}