#version 330
#extension GL_ARB_separate_shader_objects : enable

uniform sampler2D img;

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inTexture;

layout(location = 0) out vec4 fragColor;

void main()
{
	float sample = texture(img, inTexture).r;
    float distance = sample - 0.5;

    float opacity = clamp(distance / fwidth(distance) + 0.5, 0.0, 1.0);

	vec4 color = inColor;
	color.a *= opacity;

	//color.rgb = vec3(1);

    fragColor = color;
}