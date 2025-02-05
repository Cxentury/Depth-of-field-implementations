#version 330 core

in vec2 fragTexCoord;

out vec4 finalColor;

uniform sampler2D coc_texture;


void main()
{
    vec2 pixel_size = 1.0 / textureSize(coc_texture, 0);
    float maxVal = texture(coc_texture, fragTexCoord).r;

    //Horizontal pass
    for(int i = 1; i < 6; i++){
        float leftVal = texture(coc_texture, fragTexCoord + vec2(i,0) * pixel_size).r;
        float rightVal = texture(coc_texture, fragTexCoord + vec2(-i,0) * pixel_size).r;

        if(leftVal > maxVal)
            maxVal = leftVal;
        if(rightVal > maxVal)
            maxVal = rightVal;
    }

    //Vertical pass
    for(int i = 1; i < 6; i++){
        float upVal = texture(coc_texture, fragTexCoord + vec2(0,i) * pixel_size).r;
        float bottomVal = texture(coc_texture, fragTexCoord + vec2(0,-i) * pixel_size).r;

        if(upVal > maxVal)
            maxVal = upVal;
        if(bottomVal > maxVal)
            maxVal = bottomVal;
    }


    finalColor.rgb = vec3(0);
    finalColor.r = maxVal;
}