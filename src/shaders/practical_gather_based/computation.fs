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

// Circular Kernel from GPU Zen 'Practical Gather-based Bokeh Depth of Field' by Wojciech Sterna
const vec2 offsets[] = {
	2.0f * vec2(1.000000f, 0.000000f),
	2.0f * vec2(0.707107f, 0.707107f),
	2.0f * vec2(-0.000000f, 1.000000f),
	2.0f * vec2(-0.707107f, 0.707107f),
	2.0f * vec2(-1.000000f, -0.000000f),
	2.0f * vec2(-0.707106f, -0.707107f),
	2.0f * vec2(0.000000f, -1.000000f),
	2.0f * vec2(0.707107f, -0.707107f),
	
	4.0f * vec2(1.000000f, 0.000000f),
	4.0f * vec2(0.923880f, 0.382683f),
	4.0f * vec2(0.707107f, 0.707107f),
	4.0f * vec2(0.382683f, 0.923880f),
	4.0f * vec2(-0.000000f, 1.000000f),
	4.0f * vec2(-0.382684f, 0.923879f),
	4.0f * vec2(-0.707107f, 0.707107f),
	4.0f * vec2(-0.923880f, 0.382683f),
	4.0f * vec2(-1.000000f, -0.000000f),
	4.0f * vec2(-0.923879f, -0.382684f),
	4.0f * vec2(-0.707106f, -0.707107f),
	4.0f * vec2(-0.382683f, -0.923880f),
	4.0f * vec2(0.000000f, -1.000000f),
	4.0f * vec2(0.382684f, -0.923879f),
	4.0f * vec2(0.707107f, -0.707107f),
	4.0f * vec2(0.923880f, -0.382683f),

	6.0f * vec2(1.000000f, 0.000000f),
	6.0f * vec2(0.965926f, 0.258819f),
	6.0f * vec2(0.866025f, 0.500000f),
	6.0f * vec2(0.707107f, 0.707107f),
	6.0f * vec2(0.500000f, 0.866026f),
	6.0f * vec2(0.258819f, 0.965926f),
	6.0f * vec2(-0.000000f, 1.000000f),
	6.0f * vec2(-0.258819f, 0.965926f),
	6.0f * vec2(-0.500000f, 0.866025f),
	6.0f * vec2(-0.707107f, 0.707107f),
	6.0f * vec2(-0.866026f, 0.500000f),
	6.0f * vec2(-0.965926f, 0.258819f),
	6.0f * vec2(-1.000000f, -0.000000f),
	6.0f * vec2(-0.965926f, -0.258820f),
	6.0f * vec2(-0.866025f, -0.500000f),
	6.0f * vec2(-0.707106f, -0.707107f),
	6.0f * vec2(-0.499999f, -0.866026f),
	6.0f * vec2(-0.258819f, -0.965926f),
	6.0f * vec2(0.000000f, -1.000000f),
	6.0f * vec2(0.258819f, -0.965926f),
	6.0f * vec2(0.500000f, -0.866025f),
	6.0f * vec2(0.707107f, -0.707107f),
	6.0f * vec2(0.866026f, -0.499999f),
	6.0f * vec2(0.965926f, -0.258818f),
};

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