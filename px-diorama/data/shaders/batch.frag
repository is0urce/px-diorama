#version 330
#extension GL_ARB_separate_shader_objects : enable

uniform sampler2D img;

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 inTexture;

layout(location = 0) out vec4 fragColor;

const float gamma = 2.2;

void main()
{
	vec4 color = texture(img, inTexture) * inColor;

	color = pow(color, vec4(gamma));

    fragColor = color;
}