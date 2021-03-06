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
#include "../configuration.hpp"

#include <px/common/assert.hpp>
#include <px/rft/ft_font.hpp>
#include <px/ui/canvas.hpp>

#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

namespace px {

	namespace {

		float pi = 3.14f;
		float pi_2 = pi * 2;

		float lateral_r = 0.003f;
		float lateral_g = 0.001f;
		float lateral_b = 0.0f;
	}

	class renderer
	{
	public:
		void render(perception const& view)
		{
			// load data
			prepare_console(view.canvas());
			prepare_popups(view.popups());

			// update font textures
			unsigned int w, h;
			void const* font_bits;
			std::tie(font_bits, w, h) = m_ui_font.download();
			m_font_texture.image2d(GL_RED, GL_RED, w, h, 0, GL_UNSIGNED_BYTE, font_bits);

			// prepare uniforms
			float aspect = static_cast<float>(m_width) / static_cast<float>(m_height);
			glm::vec4 noize(std::rand(), std::rand(), std::rand(), std::rand());
			glm::vec2 lens_r(1 + lateral_r, 1 + lateral_r * aspect);
			glm::vec2 lens_g(1 + lateral_g, 1 + lateral_g * aspect);
			glm::vec2 lens_b(1 + lateral_b, 1 + lateral_b * aspect);
			m_camera.load<camera_uniform>(GL_STREAM_DRAW, { { view.scale(), view.scale() * aspect },{ 0.0, 0.0 } });
			m_lens.load<postprocess_uniform>(GL_STREAM_DRAW, { noize, {}, {}, {}, {}, lens_r, lens_g, lens_b, aspect });

			// g-pass
			glUseProgram(m_batch);
			auto const* batches_array = view.batches();

			px_assert(batches_array); // can have 0 size, but should exist
			px_assert(batches_array->size() == m_batches.size()); // gpu textures = drawcalls of textures

			for (size_t i = 0, total = batches_array->size(); i != total; ++i) {
				//if (i == 0)
				//{
				//	m_batches[i].pass.bind();
				//	glClearColor(1, 1, 0, 1);
				//	glClear(GL_COLOR_BUFFER_BIT);
				//}
				auto const& vertices = (*batches_array)[i];
				if (vertices.size() != 0) {
					m_batches[i].draw_arrays(GL_STREAM_DRAW, GL_QUADS, vertices.size(), vertices.data());
				}
			}

			glUseProgram(m_blur);
			m_blur_passes.draw_arrays(GL_QUADS, 4);

			glUseProgram(m_process);
			m_postprocess.draw_arrays(GL_QUADS, 4);

			glUseProgram(m_glyph);
			m_popup_pass.draw_arrays(GL_QUADS, m_popup_buffer.size());

			glUseProgram(m_console);
			m_ui_solid_pass.draw_arrays(GL_QUADS, m_ui_solid_buffer.size());

			glUseProgram(m_glyph);
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

			i.vertices = GL_ARRAY_BUFFER;
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
			, m_ui_font(grid_font_path, grid_font_size, grid_font_atlas)
			, m_popup_font(popup_font_path, popup_font_size, popup_font_atlas)
		{
			// create font texture
			for (unsigned int i = 32; i != 256; ++i) {
				m_ui_font.rasterize(i);
				m_popup_font.rasterize(i);
			}
			unsigned int w, h;
			void const* data;
			std::tie(data, w, h) = m_ui_font.download();
			m_font_texture.image2d(GL_RED, GL_RED, w, h, 0, GL_UNSIGNED_BYTE, data);
			std::tie(data, w, h) = m_popup_font.download();
			m_popup_texture.image2d(GL_RED, GL_RED, w, h, 0, GL_UNSIGNED_BYTE, data);

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
			m_process = compile_program("data/shaders/process", { "Lens" }, { "img", "blurred" });
			m_console = compile_program("data/shaders/uisolid", { "Camera" }, {});
			m_glyph = compile_program("data/shaders/uitext", { "Camera" }, { "img" });

			// ui grid
			m_ui_solid_buffer = GL_ARRAY_BUFFER;
			m_ui_text_buffer = GL_ARRAY_BUFFER;
			m_ui_solid_geometry.swizzle(m_ui_solid_buffer, sizeof(grid_vertex), { GL_FLOAT, GL_FLOAT }, { 2, 4 }, { offsetof(grid_vertex, pos), offsetof(grid_vertex, color) });
			m_ui_text_geometry.swizzle(m_ui_text_buffer, sizeof(glyph_vertex), { GL_FLOAT, GL_FLOAT, GL_FLOAT }, { 2, 2, 4 }, { offsetof(glyph_vertex, pos), offsetof(glyph_vertex, texture), offsetof(glyph_vertex, tint) });

			// popup notifications
			m_popup_buffer = GL_ARRAY_BUFFER;
			m_popup_geometry.swizzle(m_popup_buffer, sizeof(glyph_vertex), { GL_FLOAT, GL_FLOAT, GL_FLOAT }, { 2, 2, 4 }, { offsetof(glyph_vertex, pos), offsetof(glyph_vertex, texture), offsetof(glyph_vertex, tint) });
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

			m_popup_pass = { 0, m_popup_geometry, m_width, m_height };
			m_popup_pass.bind_uniform(m_camera); // only /bc pod equal!
			m_popup_pass.bind_texture(m_font_texture);
		}
		void prepare_console(ui::canvas const& canvas)
		{
			auto const& colors = canvas.colors();
			size_t grid_size = colors.size();

			m_ui_grid.clear();
			m_ui_glyph.clear();
			m_ui_grid.reserve(grid_size * 4);
			m_ui_glyph.reserve(grid_size * 4);

			canvas.background().enumerate([&](auto const& point, color const& color) {
				glm::vec4 back(color.R, color.G, color.B, color.A);
				m_ui_grid.push_back({ { point.x(), -point.y() - 1 },		back });
				m_ui_grid.push_back({ { point.x(), -point.y() },			back });
				m_ui_grid.push_back({ { point.x() + 1, -point.y() },		back });
				m_ui_grid.push_back({ { point.x() + 1, -point.y() - 1 },	back });
			});

			float mx = 1.0f / 64 / m_ui_font.width();
			float my = 1.0f / 64 / m_ui_font.height();
			canvas.codes().enumerate([&](auto const& point, auto code) {
				glm::vec4 tint(colors[point].R, colors[point].G, colors[point].B, colors[point].A);
				auto const& glyph = m_ui_font[code];
				float sx = static_cast<float>(glyph.sx);
				float sy = static_cast<float>(glyph.sy);
				float dx = static_cast<float>(glyph.dx);
				float dy = static_cast<float>(glyph.dy);
				float x = static_cast<float>(point.x()) + glyph.bearing_hx * mx;
				float y = static_cast<float>(-point.y() - 1) + glyph.bearing_hy * my;
				float w = glyph.width * mx;
				float h = glyph.height * my;
				m_ui_glyph.push_back({ { x + 0, y - h }, { sx, sy }, tint });
				m_ui_glyph.push_back({ { x + 0, y + 0 }, { sx, dy }, tint });
				m_ui_glyph.push_back({ { x + w, y + 0 }, { dx, dy }, tint });
				m_ui_glyph.push_back({ { x + w, y - h }, { dx, sy }, tint });
			});

			m_ui_uniform.load<ui_uniform>(GL_STREAM_DRAW, { { 2.0f / canvas.width(), 2.0f / canvas.height() },{ -1.0f, 1.0f } });

			m_ui_solid_buffer.load(GL_STREAM_DRAW, m_ui_grid.size() * sizeof(grid_vertex), m_ui_grid.data());
			m_ui_text_buffer.load(GL_STREAM_DRAW, m_ui_glyph.size() * sizeof(glyph_vertex), m_ui_glyph.data());
		}
		void prepare_popups(std::vector<popup> const& popups)
		{
			std::vector<glyph_vertex> glyphs;

			for (auto const& popup : popups) {
				glm::vec4 tint(popup.tint.R, popup.tint.G, popup.tint.B, popup.tint.A);
				std::string const& text = popup.text;
				float pen_x = popup.x + 0.5f;
				float pen_y = popup.y + 0.5f;
				float mult = 12.0f;
				float mx = mult / 64 / m_width;
				float my = mult / 64 / m_height;

				long text_width = 0;
				for (size_t i = 0, size = text.size(); i != size; ++i) {
					int kerning = i == 0 ? 0 : m_ui_font.kerning(text[i - 1], text[i]);
					text_width += kerning + m_ui_font[text[i]].advance_h;
				}

				pen_x -= text_width * mx / 2; // center

				for (size_t i = 0, size = text.size(); i != size; ++i) {
					auto const& glyph = m_ui_font[text[i]];

					long kerning = i == 0 ? 0 : m_ui_font.kerning(text[i - 1], text[i]);
					pen_x += (kerning >> 6) * mx;

					float sx = static_cast<float>(glyph.sx);
					float sy = static_cast<float>(glyph.sy);
					float dx = static_cast<float>(glyph.dx);
					float dy = static_cast<float>(glyph.dy);

					float x = pen_x + glyph.bearing_hx * mx;
					float y = pen_y + glyph.bearing_hy * my;
					float w = glyph.width * mx;
					float h = glyph.height * my;

					glyphs.push_back({ { x + 0, y - h }, { sx, sy }, tint });
					glyphs.push_back({ { x + 0, y + 0 }, { sx, dy }, tint });
					glyphs.push_back({ { x + w, y + 0 }, { dx, dy }, tint });
					glyphs.push_back({ { x + w, y - h }, { dx, sy }, tint });

					pen_x += glyph.advance_h * mx;
				}
			}

			m_popup_buffer.load(GL_STREAM_DRAW, sizeof(glyph_vertex) * glyphs.size(), glyphs.data());
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
			glm::vec4 noize_seed;
			glm::vec4 focus_distance;

			glm::vec2 lateral_r;
			glm::vec2 lateral_g;
			glm::vec2 lateral_b;

			glm::vec2 offset_r;
			glm::vec2 offset_g;
			glm::vec2 offset_b;

			glm::float32 aspect;
		};

	private:
		int m_width;
		int m_height;

		gl_program	m_batch;
		gl_program	m_blur;
		gl_program	m_process;
		gl_program	m_console;
		gl_program	m_glyph;

		gl_uniform	m_camera;
		gl_uniform	m_lens;

		offscreen	m_primary;
		offscreen	m_ping;
		offscreen	m_pong;

		std::vector<batch> m_batches;

		blur<4, 2> m_blur_passes;
		gl_pass m_postprocess;

		// ui console grid
		ft_font		m_ui_font;
		gl_texture	m_font_texture;
		gl_uniform	m_ui_uniform;
		gl_buffer	m_ui_solid_buffer;
		gl_buffer	m_ui_text_buffer;
		gl_vao		m_ui_solid_geometry;
		gl_vao		m_ui_text_geometry;
		gl_pass		m_ui_solid_pass;
		gl_pass		m_ui_text_pass;
		std::vector<grid_vertex> m_ui_grid;
		std::vector<glyph_vertex> m_ui_glyph;

		// popup notifications
		ft_font		m_popup_font;
		gl_texture	m_popup_texture;
		gl_buffer	m_popup_buffer;
		gl_vao		m_popup_geometry;
		gl_pass		m_popup_pass;
	};
}