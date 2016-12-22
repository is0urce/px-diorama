// name: vertex.hpp
// type: c++ header
// auth: is0urce
// desc: structures

#pragma once

// vertex data structures

#define GLM_FORCE_RADIANS
#pragma warning(push)
#pragma warning(disable:4201) // warning C4201: nonstandard extension used: nameless struct/union
#include <glm/glm.hpp>
#pragma warning(pop)

struct mesh_vertex
{
	glm::vec2 pos;
	glm::vec2 texture;
};

struct grid_vertex
{
	glm::vec2 pos;
	glm::vec4 color;
};

struct glyph_vertex
{
	glm::vec2 pos;
	glm::vec2 texture;
	glm::vec4 tint;
};