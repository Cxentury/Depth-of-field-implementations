#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;

uniform float max_blur_radius;
uniform sampler2D screen_texture;
uniform sampler2D blurred_texture;

out vec4 finalColor;

void main() {

	vec4 focus_color = texture(screen_texture,fragTexCoord);

	//CoC
	focus_color.a = abs(focus_color.a);

	finalColor = vec4(mix(focus_color.rgb,texture(blurred_texture,fragTexCoord).rgb,focus_color.a) ,1.0);
}

