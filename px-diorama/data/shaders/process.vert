#version 330
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPos;

out vec2 coord;

// counterclockwise quad

vec2 positions[4] = vec2[](
    vec2(-1.0, 1.0),
    vec2(-1.0, -1.0),
    vec2(1.0, -1.0),
	vec2(1.0, 1.0)
);

vec2 textcoords[4] = vec2[](
    vec2(0.0, 1.0),
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
	vec2(1.0, 1.0)
);

void main()
{
    gl_Position = vec4(positions[gl_VertexID], 0.0, 1.0);
	coord = textcoords[gl_VertexID];
}