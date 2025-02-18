#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D coc_texture_max;
uniform bool horizontal_pass;

void main() {

    vec2 pixelSize = 1.0 / textureSize(coc_texture_max, 0);
    
    float color = texture(coc_texture_max,fragTexCoord).r;
    float weight = 0.0;

	int radius = 6;
    vec2 offset;

	for(int i = -radius; i <= radius; i++){
        
        if(horizontal_pass){

            offset = vec2(i,0) * pixelSize;

            //Horizontal pass
            color += texture(coc_texture_max, clamp(fragTexCoord + offset, vec2(0,0), vec2(1,1))).r;
        }
        else{
            
            offset = vec2(0,i) * pixelSize;
            
            //Vertical pass
            color += texture(coc_texture_max, clamp(fragTexCoord + offset, vec2(0,0), vec2(1,1))).r;
        }
        weight+=1.0;
	}


	color /= weight;
	finalColor.r = color;
}
