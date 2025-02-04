#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D screen_texture;
uniform vec2 box_blur_settings; // separation ; blur_radius;

out vec4 finalColor;

void main() {
    float weight = 0.0;
    vec2 tex_offset = 1.0 / textureSize(screen_texture, 0);
    vec3 color = vec3(0,0,0);

	int radius = int(box_blur_settings.y);
	float separation = box_blur_settings.x;

	for(int x = -radius; x <= radius; x++){
		for(int y = -radius; y <= radius; y++){
			if(length(vec2(float(x),float(y))) > radius) continue;
			vec2 offset = vec2(float(x),float(y))*tex_offset;
            color += texture(screen_texture, fragTexCoord+offset*separation).rgb;
            weight+=1.0;
		}
	}

	color /= weight;
	finalColor = vec4(color,1.0);
}

