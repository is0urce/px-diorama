// name: renderer.hpp
// type: c++
// auth: is0urce
// desc: rendering procedures

#pragma once

#define GLM_FORCE_RADIANS
#pragma warning(push)
#pragma warning(disable:4201) // warning C4201: nonstandard extension used: nameless struct/union
#include <glm/glm.hpp>
#pragma warning(pop)

#include <px/rgl/rgl.hpp>

#include "perception.hpp"
#include "program.hpp"
#include "blur.hpp"

#include <array>
#include <stdexcept>
#include <string>
#include <vector>

static const struct triangle // keep around fallback to "don't fear the darkness" mode
{
	glm::vec2 pos;
	glm::vec3 color;
} g_vertices[3]{
	{ { -0.6f, -0.4f },{ 1.f, 1.f, 0.f } },
	{ { 0.6f, -0.4f },{ 0.f, 1.f, 1.f } },
	{ { 0.f,  0.6f },{ 1.f, 0.f, 1.f } }
};

namespace px
{
	class renderer
	{
	public:
		void render(perception const& data)
		{
			// prepare data
			m_camera.data = { { data.scale(), data.scale() * m_width / m_height },{ 0.0, 0.0 } };
			m_camera.block.load(GL_STREAM_DRAW, m_camera.data);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_offscreen);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ping.framebuffer);
			glClear(GL_COLOR_BUFFER_BIT);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_pong.framebuffer);
			glClear(GL_COLOR_BUFFER_BIT);

			// don't-fear-the-darkness pass
			//glUseProgram(m_strip);
			//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_offscreen);
			//glViewport(0, 0, m_width, m_height);
			//glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_camera.block);
			//glBindVertexArray(m_geometry);
			//glDrawArrays(GL_TRIANGLES, 0, 3);

			// g-pass
			glUseProgram(m_batch);
			for (size_t i = 0, textures = data.textures(); i != textures; ++i)
			{
				auto const& vertices = data.batch(i);
				auto size = data.size(i);
				if (size != 0)
				{
					m_batches[i].vertices.load(GL_STREAM_DRAW, sizeof(vertices[0]) * size, vertices.data());
					m_batches[i].pass.draw_arrays(GL_QUADS, static_cast<GLsizei>(size));
				}
			}

			glUseProgram(m_blur);
			m_blur_passes.draw_arrays(GL_QUADS, 4);

			glUseProgram(m_process);
			m_postprocess.draw_arrays(GL_QUADS, 4);

			gl_assert();
		}
		void resize(int width, int height)
		{
			m_width = width;
			m_height = height;
		}

	public:
		renderer(int width,	int height)
			: m_width(width), m_height(height)
		{
			m_trices = { GL_ARRAY_BUFFER };
			m_camera.block = { GL_UNIFORM_BUFFER };

			m_albedo.image2d(GL_RGBA32F, GL_RGBA, width, height);
			m_offscreen.texture(m_albedo, 0);

			m_ping.texture.image2d(GL_RGBA32F, GL_RGBA, m_width, m_height);
			m_ping.framebuffer.texture(m_ping.texture, 0);
			m_pong.texture.image2d(GL_RGBA32F, GL_RGBA, m_width, m_height);
			m_pong.framebuffer.texture(m_pong.texture, 0);

			m_geometry.swizzle(m_trices, sizeof(triangle), { GL_FLOAT, GL_FLOAT }, { 2, 3 }, { offsetof(triangle, pos), offsetof(triangle, color) });
			m_trices.load(GL_STATIC_DRAW, sizeof(g_vertices), g_vertices);

			// pipeline

			m_strip = compile_program("data/shaders/strip");
			m_strip.uniform_block("Camera", 0);

			m_batch = compile_program("data/shaders/batch");
			m_batch.uniform_block("Camera", 0);
			m_batch.uniform("img", 0);

			m_process = compile_program("data/shaders/process");
			m_process.uniform("img", 0);

			m_blur = compile_program("data/shaders/blur");
			m_blur.uniform_block("Blur", 0);
			m_blur.uniform("img", 0);

			// static passes

			m_blur_passes = { m_albedo, m_ping.framebuffer, m_ping.texture, m_pong.framebuffer, m_pong.texture, m_width, m_height, 0.5f, { 3.14f * 2 / 16, 3.14f * 2 / 16 * 3, 3.14f * 2 / 16 * 5, 3.14f * 2 / 16 * 7 } };

			m_postprocess = { 0, 0, m_width, m_height };
			m_postprocess.bind_texture(m_blur_passes.result());
		}
		void load_texture(unsigned int width, unsigned int height, void const* data)
		{
			if (!data) throw std::runtime_error("px::renderer::load_texture(...) - data is null");

			m_batches.emplace_back();
			auto & i = m_batches.back();

			i.texture.image2d(GL_RGBA, GL_RGBA, width, height, 0, GL_UNSIGNED_BYTE, data); // actual loading
			i.texture.filters(GL_NEAREST, GL_NEAREST);

			i.vertices = { GL_ARRAY_BUFFER };
			i.geometry.swizzle(i.vertices, sizeof(vertex), { GL_FLOAT, GL_FLOAT }, { 2, 2 }, { offsetof(vertex, pos), offsetof(vertex, texture) });

			i.pass = { m_offscreen, i.geometry, m_width, m_height };
			i.pass.bind_texture(i.texture);
			i.pass.bind_uniform(m_camera.block);
		}

	private:
		int m_width;
		int m_height;

		gl_program m_strip;
		gl_program m_batch;
		gl_program m_blur;
		gl_program m_process;

		gl_buffer m_trices;
		gl_vao m_geometry;

		gl_texture m_albedo;
		gl_framebuffer m_offscreen;

		struct offscreen
		{
			gl_framebuffer framebuffer;
			gl_texture texture;
		} m_ping, m_pong;

		struct batch
		{
			gl_buffer vertices;
			gl_vao geometry;
			gl_texture texture;
			gl_pass pass;
		};
		std::vector<batch> m_batches;

		struct
		{
			gl_buffer block;
			struct
			{
				glm::vec2 scale;
				glm::vec2 offset;
			} data;
		} m_camera;

		blur<4, 2> m_blur_passes;
		gl_pass m_postprocess;
	};
}