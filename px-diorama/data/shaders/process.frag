#version 330
#extension GL_ARB_separate_shader_objects : enable

layout(std140) uniform Lens
{
	vec4 distance;
	vec4 lateral;
	vec4 noise;
} lens;

uniform sampler2D img;
uniform sampler2D blured;

layout(location = 0) in vec2 coord;

layout(location = 0) out vec4 fragColor;

const vec2 r_m = vec2(1.003, 1.003);
const vec2 g_m = vec2(1.001, 1.001);
const vec2 b_m = vec2(1.0, 1.0);

//const vec2 r_o = vec2(-0.0005, -0.0005);
//const vec2 g_o = vec2(0.0005, 0.0005);
//const vec2 b_o = vec2(0.0005, 0.0);

const vec2 r_o = vec2(0.0, 0.0);
const vec2 g_o = vec2(0.0, 0.0);
const vec2 b_o = vec2(0.0, 0.0);

const float inverted_gamma = 1 / 2.2;
const float exposure = 0.05;

vec4 seek(vec2 mult, vec2 offset)
{
	vec4 tex = texture(img, (coord - vec2(0.5, 0.5)) * mult + vec2(0.5, 0.5) + offset);
	return tex;
}
vec4 seek_blured(vec2 mult, vec2 offset)
{
	vec4 tex = texture(blured, (coord - vec2(0.5, 0.5)) * mult + vec2(0.5, 0.5) + offset);
	return tex;
}

highp float rand2hi(vec2 co)
{
    highp float a = 12.9898;
    highp float b = 78.233;
    highp float c = 43758.5453;
    highp float dt= dot(co.xy ,vec2(a,b));
    highp float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

float rand2lo(vec2 co)
{
    float a = 12.9898;
    float b = 78.233;
    float c = 43758.5453;
    float dt= dot(co.xy ,vec2(a,b));
    float sn= mod(dt,3.14);
    return fract(sin(sn) * c);
}

vec3 noise3(vec3 seed)
{
	return vec3
	(
		rand2hi(vec2(gl_FragCoord.x, rand2hi(vec2(seed.r, gl_FragCoord.y)))),
		rand2hi(vec2(gl_FragCoord.y, rand2hi(vec2(seed.g, gl_FragCoord.x)))),
		rand2hi(vec2(rand2hi(gl_FragCoord.xy), seed.b))
	);
}

vec3 tonemap(vec3 chroma)
{
	return chroma / (vec3(1.0) + chroma);
}
vec3 tonemap_exposure(vec3 chroma, float exposure)
{
    return vec3(1.0) - exp(-chroma * exposure);
}

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
	vec3 grain = noise3(lens.noise.rgb) / 64;

    // tone map
	chroma = tonemap_exposure(chroma, 1);

    // gamma, convert to sRGB space
    chroma = pow(chroma, vec3(inverted_gamma));

    fragColor = vec4(chroma + grain, 1.0);
}