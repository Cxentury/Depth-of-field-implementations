#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D screen_texture;
uniform vec2 dilation_settings; // separation ; size;

out vec4 finalColor;

void main() {

    float minThreshold = .1;
    float maxThreshold = .3;
    
    vec2 tex_offset = 1.0 / textureSize(screen_texture, 0);

    finalColor.rgb = texture(screen_texture, fragTexCoord).rgb;

	int size = int(dilation_settings.y);
	float separation = dilation_settings.x;

    vec3 color_max_greyscale = finalColor.rgb;
    float max_greyscale = 0;

	for(int x = -size; x <= size; x++){
		for(int y = -size; y <= size; y++){
            if(!(distance(vec2(x,y), vec2(0,0)) <= size))
                continue;
			
            vec2 offset = vec2(float(x),float(y))*tex_offset;
            vec3 pixel_value = texture(screen_texture, fragTexCoord+offset*separation).rgb;
            float pixel_greyScale = dot(pixel_value, vec3(0.21,0.72,0.07));
            
            if(pixel_greyScale > max_greyscale){
                max_greyscale = pixel_greyScale;
                color_max_greyscale = pixel_value;
            }
		}
	}

    finalColor.a = 1.0;
	finalColor.rgb = mix(finalColor.rgb, color_max_greyscale,smoothstep(minThreshold, maxThreshold, max_greyscale));
}

