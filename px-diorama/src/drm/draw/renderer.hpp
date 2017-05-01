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

#include <px/rft/ft_library.hpp>

#include "perception.hpp"
#include "program.hpp"
#include "blur.hpp"
#include <px/rft/ft_font.hpp>
#include <px/ui/canvas.hpp>

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

namespace px {

	namespace {
		char const* ui_font_path = "data/fonts/PragmataPro.ttf";
		unsigned int ui_font_size = 18;	// size of fon
		unsigned int ui_font_atlas = 512;	// internal size of font atlas
		float pi = 3.14f;
		float pi_2 = pi * 2;
	}

	class renderer
	{
	public:
		void render(perception const& view)
		{
			// load data
			prepare_console(view.canvas());

			// prepare uniforms
			m_camera.load<camera_uniform>(GL_STREAM_DRAW, { { view.scale(), view.scale() * m_width / m_height },{ 0.0, 0.0 } });
			m_lens.load<postprocess_uniform>(GL_STREAM_DRAW, { {}, {}, glm::vec4(std::rand(), std::rand(), std::rand(), std::rand()) });

			// g-pass
			glUseProgram(m_batch);
			auto const& batches_array = view.batches();
			for (size_t i = 0, total = batches_array.size(); i != total; ++i) {
				auto const& vertices = batches_array[i];
				if (vertices.size() != 0) {
					m_batches[i].draw_arrays(GL_STREAM_DRAW, GL_QUADS, vertices.size(), vertices.data());
				}
			}

			glUseProgram(m_blur);
			m_blur_passes.draw_arrays(GL_QUADS, 4);

			glUseProgram(m_process);
			m_postprocess.draw_arrays(GL_QUADS, 4);

			glUseProgram(m_console);
			m_ui_solid_pass.draw_arrays(GL_QUADS, m_ui_solid_buffer.size());

			glUseProgram(m_glyph);
			unsigned int w, h;
			void const* font_bits = m_ui_font.download(w, h);
			m_font_texture.image2d(GL_RED, GL_RED, w, h, 0, GL_UNSIGNED_BYTE, font_bits);
			glBindTexture(GL_TEXTURE_2D, m_font_texture);
			m_ui_text_pass.draw_arrays(GL_QUADS, m_ui_text_buffer.size());

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
			: m_width(width)
			, m_height(height)
			, m_ui_font(ui_font_path, ui_font_size, ui_font_atlas)
		{
			// create font texture
			for (int i = 32; i != 256; ++i) m_ui_font.rasterize(i);
			unsigned int w, h;
			void const* data = m_ui_font.download(w, h);
			m_font_texture.image2d(GL_RED, GL_RED, w, h, 0, GL_UNSIGNED_BYTE, data);

			// setup rendering
			create_pipeline();
			create_framebuffers();
		}

	private:
		void create_pipeline()
		{
			// gl states
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);

			// shaders
			m_batch = compile_program("data/shaders/batch", { "Camera" }, { "img" });
			m_blur = compile_program("data/shaders/blur", { "Blur" }, { "img" });
			m_process = compile_program("data/shaders/process", { "Lens" }, { "img", "blured" });

			m_console = compile_program("data/shaders/uisolid", { "Camera" }, {});
			m_glyph = compile_program("data/shaders/uitext", { "Camera" }, { "img" });

			m_ui_solid_buffer = { GL_ARRAY_BUFFER };
			m_ui_text_buffer = { GL_ARRAY_BUFFER };
			m_ui_solid_geometry.swizzle(m_ui_solid_buffer, sizeof(grid_vertex), { GL_FLOAT, GL_FLOAT }, { 2, 4 }, { offsetof(grid_vertex, pos), offsetof(grid_vertex, color) });
			m_ui_text_geometry.swizzle(m_ui_text_buffer, sizeof(glyph_vertex), { GL_FLOAT, GL_FLOAT, GL_FLOAT }, { 2, 2, 4 }, { offsetof(glyph_vertex, pos), offsetof(glyph_vertex, texture), offsetof(glyph_vertex, tint) });
		}
		void create_framebuffers()
		{
			// prepare framebuffers for offscreen rendering
			m_primary.texture.image2d(GL_RGBA32F, GL_RGBA, m_width, m_height);
			m_primary.framebuffer.texture(m_primary.texture);
			m_ping.texture.image2d(GL_RGBA32F, GL_RGBA, m_width, m_height);
			m_ping.framebuffer.texture(m_ping.texture);
			m_pong.texture.image2d(GL_RGBA32F, GL_RGBA, m_width, m_height);
			m_pong.framebuffer.texture(m_pong.texture);

			// batch rendering passes setup
			for (auto & batch : m_batches) {
				batch.pass = { m_primary.framebuffer, batch.geometry, m_width, m_height };
				batch.pass.bind_texture(batch.texture);
				batch.pass.bind_uniform(m_camera);
			}

			// efx & postprocess
			m_blur_passes = { m_primary.texture, m_ping.framebuffer, m_ping.texture, m_pong.framebuffer, m_pong.texture, m_width, m_height, 0.5f,{ pi_2 / 16, pi_2 / 16 * 3, pi_2 / 16 * 5, pi_2 / 16 * 7 } };
			m_postprocess = { 0, 0, m_width, m_height };
			m_postprocess.bind_texture(m_primary.texture, 0);
			m_postprocess.bind_texture(m_blur_passes.result(), 1);
			m_postprocess.bind_uniform(m_lens);

			// ui console grid pass
			m_ui_solid_pass = { 0, m_ui_solid_geometry, m_width, m_height };
			m_ui_solid_pass.bind_uniform(m_ui_uniform);
			m_ui_text_pass = { 0, m_ui_text_geometry, m_width, m_height };
			m_ui_text_pass.bind_uniform(m_ui_uniform);
			m_ui_text_pass.bind_texture(m_font_texture);
		}
		void prepare_console(ui::canvas const& canvas)
		{
			auto const& colors = canvas.colors();
			size_t grid_size = colors.size();

			std::vector<grid_vertex> grid;
			std::vector<glyph_vertex> glyphs;
			grid.reserve(grid_size * 4);
			glyphs.reserve(grid_size * 4);

			canvas.background().enumerate([&](auto const& point, color const& color) {
				glm::vec4 back(color.R, color.G, color.B, color.A);
				grid.push_back({ { point.x(), -point.y() - 1 }, back });
				grid.push_back({ { point.x(), -point.y() }, back });
				grid.push_back({ { point.x() + 1, -point.y() }, back });
				grid.push_back({ { point.x() + 1, -point.y() - 1 }, back });
			});

			canvas.codes().enumerate([&](auto const& point, auto code) {
				glm::vec4 front_color(colors[point].R, colors[point].G, colors[point].B, colors[point].A);
				auto const& glyph = m_ui_font[code];
				float sx = static_cast<float>(glyph.sx);
				float sy = static_cast<float>(glyph.sy);
				float dx = static_cast<float>(glyph.dx);
				float dy = static_cast<float>(glyph.dy);
				glyphs.push_back({
					{ point.x(), -point.y() - 1 },
					{ sx, sy },
					front_color });
				glyphs.push_back({
					{ point.x(), -point.y() },
					{ sx, dy },
					front_color });
				glyphs.push_back({
					{ point.x() + 1, -point.y() },
					{ dx, dy },
					front_color });
				glyphs.push_back({
					{ point.x() + 1, -point.y() - 1 },
					{ dx, sy },
					front_color });
			});

			m_ui_uniform.load<ui_uniform>(GL_STREAM_DRAW, { { 2.0f / canvas.width(), 2.0f / canvas.height() },{ -1.0f, 1.0f } });

			m_ui_solid_buffer.load(GL_STREAM_DRAW, grid_size * 4 * sizeof(grid_vertex), grid.data());
			m_ui_text_buffer.load(GL_STREAM_DRAW, grid_size * 4 * sizeof(glyph_vertex), glyphs.data());
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
		struct postprocess_uniform
		{
			glm::vec4 focus_distance;
			glm::vec4 lateral;
			glm::vec4 noise;
		};

	private:
		int m_width;
		int m_height;

		gl_program m_batch;
		gl_program m_blur;
		gl_program m_process;
		gl_program m_console;
		gl_program m_glyph;

		gl_uniform m_camera;
		gl_uniform m_lens;

		offscreen m_primary;
		offscreen m_ping;
		offscreen m_pong;

		std::vector<batch> m_batches;

		blur<4, 2> m_blur_passes;
		gl_pass m_postprocess;

		// ui console grid

		ft_font m_ui_font;
		gl_texture m_font_texture;
		gl_uniform m_ui_uniform;
		gl_buffer m_ui_solid_buffer;
		gl_buffer m_ui_text_buffer;
		gl_vao m_ui_solid_geometry;
		gl_vao m_ui_text_geometry;
		gl_pass m_ui_solid_pass;
		gl_pass m_ui_text_pass;
	};
}