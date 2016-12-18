// name: batch.hpp
// type: c++ header

#pragma once

#include <px/rgl/rgl.hpp>

#include <vector>

namespace px {
	struct batch
	{
		gl_buffer vertices;
		gl_vao geometry;
		gl_texture texture;
		gl_pass pass;
	};

	struct batches
	{
		std::vector<batch> m_batches;

		void register_texture(unsigned int width, unsigned int height, void const* data)
		{
			if (!data) throw std::runtime_error("px::renderer::load_texture(...) - data is null");

			m_batches.emplace_back();
			auto & b = m_batches.back();

			b.texture.image2d(GL_RGBA, GL_RGBA, width, height, 0, GL_UNSIGNED_BYTE, data); // actual loading
			b.texture.filters(GL_NEAREST, GL_NEAREST);

			b.vertices = { GL_ARRAY_BUFFER };
			b.geometry.swizzle(i.vertices, sizeof(vertex), { GL_FLOAT, GL_FLOAT }, { 2, 2 }, { offsetof(vertex, pos), offsetof(vertex, texture) });

			b.pass = { m_primary.framebuffer, i.geometry, m_width, m_height };
			b.pass.bind_texture(i.texture, 0);
			b.pass.bind_uniform(m_camera.block);
		}

		void bind_uniform(GLuint ubo, GLuint index)
		{

		}
	};
}