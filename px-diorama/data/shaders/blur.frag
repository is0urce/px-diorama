#version 330
#extension GL_ARB_separate_shader_objects : enable

const int samples = 5;

uniform sampler2D img;

layout(std140) uniform BlurUniform
{
	vec2 direction;
	float multipliers[5];
} blur;

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
	vec4 sum = vec4(0); // result color

	vec2 start = -0.5 * float(samples - 1) * blur.direction  + inTexCoord;

	for (int i = 0; i < samples; i++)
	{
		vec2 tpos = blur.direction * float(i) + start;
		sum += texture(img, tpos) * blur.multipliers[i];
	}

    fragColor = sum;
}