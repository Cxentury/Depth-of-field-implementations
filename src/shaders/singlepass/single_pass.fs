//Code based on the article by Dennis Gustafsson https://blog.voxagon.se/2018/05/04/bokeh-depth-of-field-in-single-pass.html

#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D screen_texture; 
// uniform float far_plane; // Far plane  
uniform vec2 lens_settings;

out vec4 finalColor;

const float GOLDEN_ANGLE = 2.39996323; 
const float MAX_BLUR_SIZE = 20.0; 
const float RAD_SCALE = 0.5;

float getBlurSize(float depth, float focusPoint, float focusScale)
{
	float coc = clamp((1.0 / focusPoint - 1.0 / depth)*focusScale, -1.0, 1.0);
	return abs(coc) * MAX_BLUR_SIZE;
}

vec3 depthOfField(vec2 texCoord, float focusPoint, float focusScale)
{
	float far_plane = 100.0;

	//In the article this is multiplied by far_plane, don't understand why
	float centerDepth = texture(screen_texture, texCoord).a;

	float centerSize = getBlurSize(centerDepth, focusPoint, focusScale);
	vec3 color = texture(screen_texture, texCoord).rgb;
	float weight = 1.0;
	float radius = RAD_SCALE;
	vec2 tex_offset = 1.0 / textureSize(screen_texture, 0);

	for (float ang = 0.0; radius < MAX_BLUR_SIZE; ang += GOLDEN_ANGLE)
	{
		vec2 offset = texCoord + vec2(cos(ang), sin(ang)) * tex_offset * radius;
		vec3 sampleColor = texture(screen_texture, offset).rgb;
		float sampleDepth = texture(screen_texture, offset).a * far_plane;
		float sampleSize = getBlurSize(sampleDepth, focusPoint, focusScale);

		if (sampleDepth > centerDepth)
			sampleSize = clamp(sampleSize, 0.0, centerSize*2.0);

		float m = smoothstep(radius-0.5, radius+0.5, sampleSize);
		color += mix(color/weight, sampleColor, m);
		weight += 1.0;   
		radius += RAD_SCALE/radius;
	}

	return color /= weight;
}

void main(){
	finalColor.a = 1.0;
	finalColor.rgb = depthOfField(fragTexCoord,lens_settings.x,lens_settings.y);
}