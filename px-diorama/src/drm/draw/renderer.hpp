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
#include "distance_font.hpp"

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
			// prepare uniform data
			m_camera.load<camera_uniform>(GL_STREAM_DRAW, { { data.scale(), data.scale() * m_width / m_height },{ 0.0, 0.0 } });
			m_ui_uniform.load<ui_uniform>(GL_STREAM_DRAW, { { 0.1f, 0.1f }, { -1.0f, 1.0f } });

			// g-pass
			glUseProgram(m_batch);
			for (size_t i = 0, total = data.textures(); i != total; ++i)
			{
				auto const& vertices = data.batch(i);
				auto size = vertices.size();
				if (size != 0)
				{
					m_batches[i].draw_arrays(GL_STREAM_DRAW, GL_QUADS, size, vertices.data());
				}
			}

			glUseProgram(m_blur);
			m_blur_passes.draw_arrays(GL_QUADS, 4);

			glUseProgram(m_process);
			m_postprocess.draw_arrays(GL_QUADS, 4);

			glUseProgram(m_console);
			auto const& bg = data.canvas().background();
			size_t size = bg.size();
			std::vector<grid_vertex> grid;
			grid.reserve(size * 4);
			bg.enumerate([&](auto const& point, color const& color) {
				grid.push_back({ { point.x(), -point.y() - 1},{ color.R, color.G, color.B, color.A } });
				grid.push_back({ { point.x(), -point.y() },{ color.R, color.G, color.B, color.A } });
				grid.push_back({ { point.x() + 1, -point.y() },{ color.R, color.G, color.B, color.A } });
				grid.push_back({ { point.x() + 1, -point.y() - 1},{ color.R, color.G, color.B, color.A } });
			});
			m_console_buffer.load(GL_STREAM_DRAW, size * 4 * sizeof(grid_vertex), grid.data());
			m_ui_solid_pass.draw_arrays(GL_QUADS, 4 * static_cast<GLsizei>(size));

			glUseProgram(m_text);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glViewport(0, 0, m_width, m_height);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_font_texture);
			glDrawArrays(GL_QUADS, 0, 4);

			gl_assert();
		}
		void add_texture(unsigned int width, unsigned int height, void const* data)
		{
			if (!data) throw std::runtime_error("px::renderer::load_texture(...) - data is null");

			m_batches.emplace_back();
			auto & i = m_batches.back();

			i.texture.image2d(GL_RGBA, GL_RGBA, width, height, 0, GL_UNSIGNED_BYTE, data); // actual loading
			i.texture.filters(GL_NEAREST, GL_NEAREST); // required

			i.vertices = { GL_ARRAY_BUFFER };
			i.geometry.swizzle(i.vertices, sizeof(mesh_vertex), { GL_FLOAT, GL_FLOAT }, { 2, 2 }, { offsetof(mesh_vertex, pos), offsetof(mesh_vertex, texture) });

			i.pass = { m_primary.framebuffer, i.geometry, m_width, m_height };
			i.pass.bind_texture(i.texture);
			i.pass.bind_uniform(m_camera);
		}
		void resize(int width, int height)
		{
			m_width = width;
			m_height = height;

			create_framebuffers();
		}

	public:
		renderer(int width, int height)
			: m_width(width), m_height(height)
			, m_font("data/fonts/DejaVuSansMono.ttf", 6, 11)
		{
			create_pipeline();
			create_framebuffers();

			for (int i = 32; i != 128; ++i) m_font.load(i);
			unsigned int w, h;
			float const* data = m_font.update(w, h);
			m_font_texture.image2d(GL_RED, GL_RED, w, h, 0, GL_FLOAT, data);
			m_font_texture.filters(GL_LINEAR, GL_LINEAR);
		}

	private:
		void create_pipeline()
		{
			// gl states
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);

			// shaders
			m_batch = compile_program("data/shaders/batch", { "Camera" }, { "img" });
			m_process = compile_program("data/shaders/process", {}, { "img" });
			m_blur = compile_program("data/shaders/blur", { "Blur" }, { "img" });
			m_console = compile_program("data/shaders/uisolid", { "Camera" }, {});
			m_text = compile_program("data/shaders/text", {}, { "img" });

			m_console_buffer = { GL_ARRAY_BUFFER };
			m_ui_solid_geometry.swizzle(m_console_buffer, sizeof(grid_vertex), { GL_FLOAT, GL_FLOAT }, { 2, 4 }, { offsetof(grid_vertex, pos), offsetof(grid_vertex, color) });
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

			// passes

			for (auto& batch : m_batches)
			{
				batch.pass = { m_primary.framebuffer, batch.geometry, m_width, m_height };
				batch.pass.bind_texture(batch.texture);
				batch.pass.bind_uniform(m_camera);
			}

			m_blur_passes = { m_primary.texture, m_ping.framebuffer, m_ping.texture, m_pong.framebuffer, m_pong.texture, m_width, m_height, 0.5f,{ 3.14f * 2 / 16, 3.14f * 2 / 16 * 3, 3.14f * 2 / 16 * 5, 3.14f * 2 / 16 * 7 } };

			m_postprocess = { 0, 0, m_width, m_height };
			//m_postprocess.bind_texture(m_blur_passes.result());
			m_postprocess.bind_texture(m_primary.texture);

			m_ui_solid_pass = { 0, m_ui_solid_geometry, m_width, m_height };
			m_ui_solid_pass.bind_uniform(m_ui_uniform);
		}

	private:
		struct offscreen
		{
			gl_framebuffer framebuffer;
			gl_texture texture;
		};
		struct batch
		{
			gl_buffer vertices;
			gl_vao geometry;
			gl_texture texture;
			gl_pass pass;

			template <typename T>
			void draw_arrays(GLenum usage, GLenum mode, size_t size, T const* data)
			{
				vertices.load(usage, sizeof(T) * size, data);
				pass.draw_arrays(mode, static_cast<GLsizei>(size));
			}
		};
		struct camera_uniform
		{
			glm::vec2 scale;
			glm::vec2 offset;
		};
		struct ui_uniform
		{
			glm::vec2 scale;
			glm::vec2 offset;
		};

	private:
		int m_width;
		int m_height;

		gl_program m_batch;
		gl_program m_blur;
		gl_program m_process;
		gl_program m_console;
		gl_program m_text;

		gl_uniform m_camera;
		gl_uniform m_ui_uniform;

		gl_buffer m_console_buffer;
		gl_vao m_ui_solid_geometry;
		gl_pass m_ui_solid_pass;

		offscreen m_primary;
		offscreen m_ping;
		offscreen m_pong;

		std::vector<batch> m_batches;

		blur<4, 2> m_blur_passes;
		gl_pass m_postprocess;

		distance_font m_font;
		gl_texture m_font_texture;
	};
}