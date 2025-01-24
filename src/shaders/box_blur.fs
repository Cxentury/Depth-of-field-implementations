#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;

uniform float max_blur_radius;
uniform float separation;
uniform vec2 box_blur_settings; //(max_blur_radius; separation);
out vec4 finalColor;

void main() {
    // finalColor = vec4(texture(coc_texture,fragTexCoord).rgb,1.0);
    vec3 color = texture(coc_texture, fragTexCoord).rgb;
    vec2 tex_offset = 1.0 / textureSize(coc_texture, 0);

	float coc = texture(coc_texture, fragTexCoord).a;
    float weight = 1;

	int radius = int(abs(coc)*max_blur_radius);

	for(int x = -radius; x <= radius; x++){
		for(int y = -radius; y <= radius; y++){
			vec2 offset = vec2(float(x),float(y))*tex_offset;
            color += texture(coc_texture, fragTexCoord+offset*separation).rgb;
            weight+=1.0;
		}
	}

	color /= weight;
	finalColor = vec4(color,1.0);
}

