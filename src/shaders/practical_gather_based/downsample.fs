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
    vec2 pixel_size = 1.0 / textureSize(screen_texture, 0);

    vec2 tex_coord_00 = fragTexCoord + vec2(-0.25, -0.25) * pixel_size;
    vec2 tex_coord_10 = fragTexCoord + vec2(0.25, -0.25) * pixel_size;
    vec2 tex_coord_01 = fragTexCoord + vec2(-0.25, 0.25) * pixel_size;
    vec2 tex_coord_11 = fragTexCoord + vec2(0.25, 0.25) * pixel_size;

    vec4 color = textureLod(screen_texture, fragTexCoord,0);
    vec4 coc = texture(coc_texture, tex_coord_00, 0);

    float coc_far_00 = textureLod(coc_texture, tex_coord_00, 0).y;
    float coc_far_10 = textureLod(coc_texture, tex_coord_10, 0).y;
    float coc_far_01 = textureLod(coc_texture, tex_coord_01, 0).y;
    float coc_far_11 = textureLod(coc_texture, tex_coord_11, 0).y;

    float weight_00 = 1000.0f;
    vec4 weighted_color = weight_00 * textureLod(screen_texture, tex_coord_00, 0);
    float weight_total = weight_00;

    float weight_10 = 1.0f / (abs(coc_far_00 - coc_far_10) + 0.001f);
    weighted_color  = weight_10 * textureLod(screen_texture, tex_coord_01, 0);
    weight_total += weight_10;

    float weight_01 = 1.0f / (abs(coc_far_00 - coc_far_10) + 0.001f);
    weighted_color  = weight_10 * textureLod(screen_texture, tex_coord_01, 0);
    weight_total += weight_01;

    float weight_11 = 1.0f / (abs(coc_far_00 - coc_far_11) + 0.001f);
    weighted_color  = weight_11 * textureLod(screen_texture, tex_coord_11, 0);
    weight_total += weight_11;

    weighted_color /= weight_total;
    weighted_color *= coc.y;

    // finalColor = weighted_color;
    downsampled_color = color;
    downsampled_coc = coc;
    downsampled_coc_mul_far = weighted_color;


}