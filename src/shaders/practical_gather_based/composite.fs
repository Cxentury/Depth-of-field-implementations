#version 430 core

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform float blend;
uniform sampler2D screen_texture;
uniform sampler2D coc_texture;
uniform sampler2D downsampled_coc;
// uniform sampler2D coc_near_blurred;

// uniform sampler2D near_field;
uniform sampler2D far_field;

void main(){

    vec4 result = textureLod(screen_texture, fragTexCoord, 0);

    vec2 pixelSize = 1.0 / textureSize(screen_texture, 0);

    vec2 texCoord00 = fragTexCoord;
    vec2 texCoord10 = fragTexCoord + vec2(pixelSize.x, 0.0);
    vec2 texCoord01 = fragTexCoord + vec2(0.0, pixelSize.y);
    vec2 texCoord11 = fragTexCoord + vec2(pixelSize.x, pixelSize.y);


	float cocFar = textureLod(coc_texture, fragTexCoord, 0).y;
	vec4 cocFarDownsampled = textureGather(downsampled_coc, texCoord00, 1).wzxy;
    vec4 cocsFarDiffs = abs(cocFar.xxxx - cocFarDownsampled);

    vec4 dofFar00 = textureLod(far_field, texCoord00, 0);
    vec4 dofFar10 = textureLod(far_field, texCoord10, 0);
    vec4 dofFar01 = textureLod(far_field, texCoord01, 0);
    vec4 dofFar11 = textureLod(far_field, texCoord11, 0);

    //Bilinear filtering weights
    vec2 fractional = fract(fragTexCoord / pixelSize);
    float a = (1.0f - fractional.x) * (1.0f - fractional.y);
    float b = fractional.x * (1.0f - fractional.y);
    float c = (1.0f - fractional.x) * fractional.y;
    float d = fractional.x * fractional.y;

    vec4 dofFar = vec4(.0);
    float weight = .0;

    float weight00 = a / (cocsFarDiffs.x + 0.001f);
    dofFar += weight00 * dofFar00;
    weight += weight00;

    float weight10 = b / (cocsFarDiffs.y + 0.001f);
    dofFar += weight10 * dofFar10;
    weight += weight10;
    
    float weight01 = c / (cocsFarDiffs.z + 0.001f);
    dofFar += weight01 * dofFar01;
    weight += weight01;

    float weight11 = d / (cocsFarDiffs.w + 0.001f);
    dofFar += weight11 * dofFar11;
    weight += weight11;

    dofFar /= weight;

    result = mix(result, dofFar, cocFar * blend);
    finalColor = result;
    // float cocNear = textureLod(coc_near_blurred, fragTexCoord, 0).x;
    // vec4 dofNear = textureLod(near_field, fragTexCoord, 0);

    // result = mix(result, dofNear, cocNear);
    // finalColor = result;
}