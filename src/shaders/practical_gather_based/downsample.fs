#version 330 core

layout(location = 0) out vec4 downsampled_color;
layout(location = 1) out vec4 downsampled_coc;
layout(location = 2) out vec4 downsampled_coc_mul_far;

in vec2 fragTexCoord;

uniform sampler2D screen_texture;
uniform sampler2D coc_texture;

//I thought _ was more readable than Caps, but I regret now
void main()
{
    vec2 pixelSize = 1.0 / textureSize(screen_texture, 0);

    vec2 texCoord00 = fragTexCoord + vec2(-0.25, -0.25) * pixelSize;
    vec2 texCoord10 = fragTexCoord + vec2(0.25, -0.25) * pixelSize;
    vec2 texCoord01 = fragTexCoord + vec2(-0.25, 0.25) * pixelSize;
    vec2 texCoord11 = fragTexCoord + vec2(0.25, 0.25) * pixelSize;

    vec4 color = textureLod(screen_texture, fragTexCoord,0);
    vec4 coc = texture(coc_texture, texCoord00, 0);

    float cocFar00 = textureLod(coc_texture, texCoord00, 0).y;
    float cocFar10 = textureLod(coc_texture, texCoord10, 0).y;
    float cocFar01 = textureLod(coc_texture, texCoord01, 0).y;
    float cocFar11 = textureLod(coc_texture, texCoord11, 0).y;

    float weight00 = 1000.0;
    vec4 weightedColor = weight00 * textureLod(screen_texture, texCoord00, 0);
    float weightTotal = weight00;

    float weight10 = 1.0 / (abs(cocFar00 - cocFar10) + 0.001);
    weightedColor += weight10 * textureLod(screen_texture, texCoord10, 0);
    weightTotal += weight10;

    float weight01 = 1.0 / (abs(cocFar00 - cocFar01) + 0.001);
    weightedColor += weight01 * textureLod(screen_texture, texCoord01, 0);
    weightTotal += weight01;

    float weight11 = 1.0 / (abs(cocFar00 - cocFar11) + 0.001);
    weightedColor += weight11 * textureLod(screen_texture, texCoord11, 0);
    weightTotal += weight11;

    weightedColor /= weightTotal;
    weightedColor *= coc.y;

    // finalColor = weightedColor;
    downsampled_color = color;
    downsampled_coc = coc;
    downsampled_coc_mul_far = weightedColor;


}