#version 330 core

layout(location = 0) out vec4 near_field;
layout(location = 1) out vec4 far_field;

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform float kernel_scale;
uniform sampler2D downsampled_color;
uniform sampler2D downsampled_coc;
uniform sampler2D downsampled_coc_mul_far;
uniform sampler2D coc_near_blurred;
	
// Circular Kernel from GPU Zen 'Practical Gather-based Bokeh Depth of Field' by Wojciech 
// const vec2 offsets[2] = vec2[2](2.0*vec2(1.0,0.0),2.0*vec2(1.0,0.0));	
const vec2 offsets[48] = vec2[48](
	2.0 * vec2(1.000000, 0.000000),
	2.0 * vec2(0.707107, 0.707107),
	2.0 * vec2(-0.000000, 1.000000),
	2.0 * vec2(-0.707107, 0.707107),
	2.0 * vec2(-1.000000, -0.000000),
	2.0 * vec2(-0.707106, -0.707107),
	2.0 * vec2(0.000000, -1.000000),
	2.0 * vec2(0.707107, -0.707107),
	
	4.0 * vec2(1.000000, 0.000000),
	4.0 * vec2(0.923880, 0.382683),
	4.0 * vec2(0.707107, 0.707107),
	4.0 * vec2(0.382683, 0.923880),
	4.0 * vec2(-0.000000, 1.000000),
	4.0 * vec2(-0.382684, 0.923879),
	4.0 * vec2(-0.707107, 0.707107),
	4.0 * vec2(-0.923880, 0.382683),
	4.0 * vec2(-1.000000, -0.000000),
	4.0 * vec2(-0.923879, -0.382684),
	4.0 * vec2(-0.707106, -0.707107),
	4.0 * vec2(-0.382683, -0.923880),
	4.0 * vec2(0.000000, -1.000000),
	4.0 * vec2(0.382684, -0.923879),
	4.0 * vec2(0.707107, -0.707107),
	4.0 * vec2(0.923880, -0.382683),

	6.0 * vec2(1.000000, 0.000000),
	6.0 * vec2(0.965926, 0.258819),
	6.0 * vec2(0.866025, 0.500000),
	6.0 * vec2(0.707107, 0.707107),
	6.0 * vec2(0.500000, 0.866026),
	6.0 * vec2(0.258819, 0.965926),
	6.0 * vec2(-0.000000, 1.000000),
	6.0 * vec2(-0.258819, 0.965926),
	6.0 * vec2(-0.500000, 0.866025),
	6.0 * vec2(-0.707107, 0.707107),
	6.0 * vec2(-0.866026, 0.500000),
	6.0 * vec2(-0.965926, 0.258819),
	6.0 * vec2(-1.000000, -0.000000),
	6.0 * vec2(-0.965926, -0.258820),
	6.0 * vec2(-0.866025, -0.500000),
	6.0 * vec2(-0.707106, -0.707107),
	6.0 * vec2(-0.499999, -0.866026),
	6.0 * vec2(-0.258819, -0.965926),
	6.0 * vec2(0.000000, -1.000000),
	6.0 * vec2(0.258819, -0.965926),
	6.0 * vec2(0.500000, -0.866025),
	6.0 * vec2(0.707107, -0.707107),
	6.0 * vec2(0.866026, -0.499999),
	6.0 * vec2(0.965926, -0.258818)
);

vec4 near(){

	vec2 pixelSize = 1.0 / textureSize(downsampled_color, 0);
	vec4 result = vec4(0);

	for(int i = 0; i < 48; i++){

		vec2 offset = kernel_scale * offsets[i] * pixelSize;
		vec4 sample = textureLod(downsampled_color, fragTexCoord + offset, 0);

		result += sample;
	}

	return result / 48.0;
}


vec4 far(){

	vec2 pixelSize = 1.0 / textureSize(downsampled_color, 0);
	vec4 result = textureLod(downsampled_coc_mul_far,fragTexCoord,0);
	float weight = textureLod(downsampled_coc, fragTexCoord,0).y;

	for(int i = 0; i < 48; i++){

		vec2 offset = kernel_scale * offsets[i] * pixelSize;

		float coc = textureLod(downsampled_coc, fragTexCoord + offset, 0).y;
		vec4 sample = textureLod(downsampled_coc_mul_far, fragTexCoord + offset, 0);

		result += sample;
		weight += coc;
	}

	return result / weight;
}

void main(){

	float cocNearBlurrred = textureLod(coc_near_blurred, fragTexCoord, 0).r;
	float cocFar = textureLod(downsampled_coc, fragTexCoord, 0).y;
	vec4 color = textureLod(downsampled_color, fragTexCoord, 0);

	if(cocNearBlurrred > 0.0)
		near_field = near();
	else
		near_field = color;

	if(cocFar > 0.0)
		far_field = far();
	else
		far_field = vec4(0.0);
}