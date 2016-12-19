#version 330
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTexture
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outTexture;

void main()
{
    gl_Position = vec4(inPos, 0.0, 1.0);
	outColor = inColor;
	outTexture = inTexture
}