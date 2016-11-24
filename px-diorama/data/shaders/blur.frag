#version 330
#extension GL_ARB_separate_shader_objects : enable

const int samplerange = 2;

uniform sampler2D img;

layout(std140) uniform Blur
{
	vec2 direction;
	float bokeh;
	float padding;
	float multipliers[samplerange + 1];
} blur;

layout(location = 0) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;

void main()
{
	vec4 sum = texture(img, inTexCoord) * blur.multipliers[0];
	vec4 maximum = texture(img, inTexCoord);

	for (int i = 0; i < samplerange; i++)
	{
		vec2 tpos = blur.direction * float(i + 1) + inTexCoord;

		vec4 color = texture(img, tpos);
		sum += color * blur.multipliers[i + 1];
		maximum = max(maximum, color);
	}
	for (int i = 0; i < samplerange; i++)
	{
		vec2 tpos = -blur.direction * float(i + 1) + inTexCoord;

		vec4 color = texture(img, tpos);
		sum += color * blur.multipliers[i + 1];
		maximum = max(maximum, color);
	}

    fragColor = mix(sum, maximum, blur.bokeh);
}