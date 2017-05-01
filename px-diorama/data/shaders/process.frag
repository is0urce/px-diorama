#version 330
#extension GL_ARB_separate_shader_objects : enable

uniform sampler2D img;
uniform sampler2D blured;

layout(location = 0) in vec2 coord;

layout(location = 0) out vec4 fragColor;

vec4 peek(vec2 offset)
{
	return texture(img, coord + offset);
}

vec4 poll(vec2 mult)
{
	return texture(img, (coord - vec2(0.5, 0.5)) * mult + vec2(0.5, 0.5));
}

vec4 seek(vec2 mult, vec2 offset)
{
	return texture(img, (coord - vec2(0.5, 0.5)) * mult + vec2(0.5, 0.5) + offset);
}
vec4 seek_blured(vec2 mult, vec2 offset)
{
	return texture(blured, (coord - vec2(0.5, 0.5)) * mult + vec2(0.5, 0.5) + offset);
}

vec2 translate(vec3 r) // r is for refraction
{
	float s = ( -r.z / r.x + 1 ) / 2.0;
	float t = ( -r.y / r.x + 1 ) / 2.0;
	return vec2(s, t);
}

const vec2 r_m = vec2(1.003, 1.003);
const vec2 g_m = vec2(1.001, 1.001);
const vec2 b_m = vec2(1.0, 1.0);

const vec2 r_o = vec2(-0.0005, -0.0005);
const vec2 g_o = vec2(0.0005, 0.0005);
const vec2 b_o = vec2(0.0005, 0.0);

//const vec2 r_o = vec2(0.0, 0.0);
//const vec2 g_o = vec2(0.0, 0.0);
//const vec2 b_o = vec2(0.0, 0.0);

const float inverted_gamma = 1 / 2.2;
const float exposure = 1.0;

void main()
{
	float distance = pow(coord.x - 0.5, 2) + pow((coord.y * 2 - 1), 2);
	float blur_factor = min(1.0, sqrt(distance));
	
	float r_blur = blur_factor * 0.5;
	float g_blur = blur_factor * 0.25;
	float b_blur = blur_factor;

    float r = seek(r_m, r_o).r * (1.0 - r_blur) + seek_blured(r_m, r_o).r * r_blur;
    float g = seek(g_m, g_o).g * (1.0 - g_blur) + seek_blured(r_m, r_o).g * g_blur;
	float b = seek(b_m, b_o).b * (1.0 - b_blur) + seek_blured(r_m, r_o).b * b_blur;

    vec3 chroma = vec3(r, g, b);

    // tone map
    vec3 result = vec3(1.0) - exp(-chroma * exposure);

    // gamma
    result = pow(result, vec3(inverted_gamma));

    fragColor = vec4(r, g, b, 1.0);
}