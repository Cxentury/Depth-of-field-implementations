#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;

uniform float max_blur_radius;
uniform sampler2D screen_texture;
uniform sampler2D blurred_texture;

out vec4 finalColor;

void main() {

    // vec3 color = texture(screen_texture, fragTexCoord).rgb;
    // vec2 tex_offset = 1.0 / textureSize(screen_texture, 0);

	// float coc = texture(screen_texture, fragTexCoord).a;
    // float weight = 1;

	// int radius = int(abs(coc)*max_blur_radius);
	// for(int x = -radius; x <= radius; x++){
	// 	for(int y = -radius; y <= radius; y++){
	// 		vec2 offset = vec2(float(x),float(y))*tex_offset;
	// 		float coc_sample = texture(screen_texture, fragTexCoord+offset).a;
	// 		if(coc_sample >= length(offset)){
	// 			color += texture(screen_texture, fragTexCoord+offset).rgb;
	// 			weight+=1.0;
	// 		}
	// 	}
	// }
	// color *= 1.0/weight;
	// finalColor = vec4(color,1.0);
	vec4 focus_color = texture(screen_texture,fragTexCoord);

	//CoC
	focus_color.a = abs(focus_color.a);
	// focus_color.a = max(0,focus_color.a - 0.2);
	
	finalColor = vec4(mix(focus_color.rgb,texture(blurred_texture,fragTexCoord).rgb,focus_color.a) ,1.0);
}

