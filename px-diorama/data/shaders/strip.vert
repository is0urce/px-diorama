#version 330
#extension GL_ARB_separate_shader_objects : enable

layout(std140) uniform Camera
{
	vec2 scale;
	vec2 offset;
} camera;

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;

void main()
{
    gl_Position = vec4(camera.scale * inPos + camera.offset, 0.0, 1.0);
    outColor = inColor;
}