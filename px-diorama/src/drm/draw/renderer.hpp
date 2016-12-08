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

#include <stdexcept>
#include <string>
#include <vector>

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

			// g-pass
			glUseProgram(m_batch);
			for (size_t i = 0, total = data.textures(); i != total; ++i)
			{
				auto const& vertices = data.batch(i);
				auto size = vertices.size();
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
		void load_texture(unsigned int width, unsigned int height, void const* data)
		{
			if (!data) throw std::runtime_error("px::renderer::load_texture(...) - data is null");

			m_batches.emplace_back();
			auto & i = m_batches.back();

			i.texture.image2d(GL_RGBA, GL_RGBA, width, height, 0, GL_UNSIGNED_BYTE, data); // actual loading
			i.texture.filters(GL_NEAREST, GL_NEAREST);

			i.vertices = { GL_ARRAY_BUFFER };
			i.geometry.swizzle(i.vertices, sizeof(vertex), { GL_FLOAT, GL_FLOAT }, { 2, 2 }, { offsetof(vertex, pos), offsetof(vertex, texture) });

			i.pass = { m_primary.framebuffer, i.geometry, m_width, m_height };
			i.pass.bind_texture(i.texture);
			i.pass.bind_uniform(m_camera.block);
		}
		void resize(int width, int height)
		{
			m_width = width;
			m_height = height;

			create_framebuffers();
		}

	public:
		renderer(int width,	int height)
			: m_width(width), m_height(height)
		{
			create_pipeline();
			create_framebuffers();
		}

	private:
		void create_pipeline()
		{
			// gl states
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);

			// uniforms
			m_camera.block = { GL_UNIFORM_BUFFER };

			// shaders
			m_batch = compile_program("data/shaders/batch");
			m_batch.uniform_block("Camera", 0);
			m_batch.uniform("img", 0);

			m_process = compile_program("data/shaders/process");
			m_process.uniform("img", 0);

			m_blur = compile_program("data/shaders/blur");
			m_blur.uniform_block("Blur", 0);
			m_blur.uniform("img", 0);

		}
		void create_framebuffers()
		{
			// framebuffers

			m_primary.texture.image2d(GL_RGBA32F, GL_RGBA, m_width, m_height);
			m_primary.framebuffer.texture(m_primary.texture);
			m_ping.texture.image2d(GL_RGBA32F, GL_RGBA, m_width, m_height);
			m_ping.framebuffer.texture(m_ping.texture);
			m_pong.texture.image2d(GL_RGBA32F, GL_RGBA, m_width, m_height);
			m_pong.framebuffer.texture(m_pong.texture);

			// static passes

			m_blur_passes = { m_primary.texture, m_ping.framebuffer, m_ping.texture, m_pong.framebuffer, m_pong.texture, m_width, m_height, 0.5f,{ 3.14f * 2 / 16, 3.14f * 2 / 16 * 3, 3.14f * 2 / 16 * 5, 3.14f * 2 / 16 * 7 } };

			m_postprocess = { 0, 0, m_width, m_height };
			//m_postprocess.bind_texture(m_blur_passes.result());
			m_postprocess.bind_texture(m_primary.texture);

			// batches
			for (auto& batch : m_batches)
			{
				batch.pass = { m_primary.framebuffer, batch.geometry, m_width, m_height };
				batch.pass.bind_texture(batch.texture);
				batch.pass.bind_uniform(m_camera.block);
			}
		}

	private:
		int m_width;
		int m_height;

		gl_program m_batch;
		gl_program m_blur;
		gl_program m_process;

		struct offscreen
		{
			gl_framebuffer framebuffer;
			gl_texture texture;
		} m_primary, m_ping, m_pong;

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