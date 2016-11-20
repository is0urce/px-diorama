#version 330
#extension GL_ARB_separate_shader_objects : enable

uniform sampler2D img;

in vec2 coord;

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

vec2 translate(vec3 r) // r is for refraction
{
	float s = ( -r.z / r.x + 1 ) / 2.0;
	float t = ( -r.y / r.x + 1 ) / 2.0;
	return vec2(s, t);
}

void main()
{
    float g = seek(vec2(1.0, 1.0), vec2(0.001, 0.001)).g;
    float r = seek(vec2(1.001, 1.001), vec2(-0.001, -0.001)).r; 
	float b = seek(vec2(0.999, 0.999), vec2(0.001, 0.0)).b;

    fragColor = vec4(r, g, b, 1.0);
}