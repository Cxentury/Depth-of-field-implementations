#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D coc_texture;
uniform sampler2D coc_texture_max;

uniform bool horizontal_pass;

void main()
{
    vec2 pixelSize = 1.0 / textureSize(coc_texture, 0);
    float maxVal = texture(coc_texture, fragTexCoord).r;

    float plusVal = 0;
    float minVal = 0;
    
    vec2 offset;

    for(int i = 1; i <= 6; i++){

        
        if(horizontal_pass){

            offset = vec2(i,0) * pixelSize;

            //Horizontal pass
            plusVal = texture(coc_texture, clamp(fragTexCoord + offset, vec2(0,0), vec2(1,1))).r;
            minVal = texture(coc_texture, clamp(fragTexCoord - offset, vec2(0,0), vec2(1,1))).r;
        }
        else{
            offset = vec2(0,i) * pixelSize;

            //Vertical pass
            plusVal = texture(coc_texture_max, clamp(fragTexCoord + offset, vec2(0,0), vec2(1,1))).r;
            minVal = texture(coc_texture_max, clamp(fragTexCoord - offset, vec2(0,0), vec2(1,1))).r;
        }

        maxVal = max(maxVal,plusVal);
        maxVal = max(maxVal,minVal);
    }

    finalColor.r = maxVal;
}