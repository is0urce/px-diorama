// name: renderer.hpp
// type: c++
// auth: is0urce
// desc: rendering procedures

#define GLM_FORCE_RADIANS
#pragma warning(push)
#pragma warning(disable:4201) // warning C4201: nonstandard extension used: nameless struct/union
#include <glm/glm.hpp>
#pragma warning(pop)

#include <px/rgl/gl_program.hpp>
#include <px/rgl/gl_buffer.hpp>
#include <px/rgl/gl_vao.hpp>
#include <px/rgl/gl_texture.hpp>
#include <px/rgl/gl_framebuffer.hpp>
#include <px/rgl/gl_pass.hpp>

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
	struct vertex
	{
		glm::vec2 pos;
		glm::vec2 texture;
	};

	class renderer
	{
	public:
		void render()
		{
#if _DEBUG
			GLenum err = GL_NO_ERROR;
			while ((err = glGetError()) != GL_NO_ERROR)	throw std::runtime_error("OpenGL error #" + std::to_string(err) + reinterpret_cast<char const*>(glewGetErrorString(err)));
#endif
			// prepare data
			m_camera.data = { { 1.2f, 1.2f },{ 0.0, 0.0 } };
			m_camera.block.load(GL_STREAM_DRAW, m_camera.data);

			// g-pass
			glUseProgram(m_batch);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_offscreen);
			glViewport(0, 0, m_width, m_height);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_camera.block);
			glBindVertexArray(m_geometry);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			glUseProgram(m_blur);
			m_blur_passes.draw_arrays(GL_QUADS, 4);

			glUseProgram(m_process);
			m_postprocess.draw_arrays(GL_QUADS, 4);
		}
		void resize(int width, int height)
		{
			m_width = width;
			m_height = height;
		}

	public:
		renderer(int width,	int height)
			: m_width(width), m_height(height)
			, m_geometry{ true }
		{
			m_trices = { GL_ARRAY_BUFFER };
			m_camera.block = { GL_UNIFORM_BUFFER };

			m_albedo.image2d(GL_RGBA32F, GL_RGBA, width, height);
			m_offscreen.texture(m_albedo, 0);

			m_ping.texture.image2d(GL_RGBA32F, GL_RGBA, m_width, m_height);
			m_ping.framebuffer.texture(m_ping.texture, 0);
			m_pong.texture.image2d(GL_RGBA32F, GL_RGBA, m_width, m_height);
			m_pong.framebuffer.texture(m_pong.texture, 0);

			m_trices.load(GL_STATIC_DRAW, sizeof(g_vertices), g_vertices);
			m_geometry.swizzle(m_trices, sizeof(triangle), { GL_FLOAT, GL_FLOAT }, { 2, 3 }, { offsetof(triangle, pos), offsetof(triangle, color) });

			// pipeline

			m_batch = compile_program("data/shaders/strip");
			m_batch.uniform_block("Camera", 0);

			m_process = compile_program("data/shaders/process");
			m_process.uniform("img", 0);

			m_blur = compile_program("data/shaders/blur");
			m_blur.uniform_block("Blur", 0);
			m_blur.uniform("img", 0);

			// passes

			m_blur_passes = { m_albedo, m_ping.framebuffer, m_ping.texture, m_pong.framebuffer, m_pong.texture, m_width, m_height, 0.5f, { 0.0f, 2.0f, 4.0f } };

			m_postprocess = { 0, m_width, m_height };
			m_postprocess.bind_texture(m_blur_passes.result());
		}
		void load_texture(unsigned int width, unsigned int height, void const* data)
		{
			if (!data) throw std::runtime_error("px::renderer::load_texture(...) - data is null");

			m_batches.emplace_back();
			auto & batch = m_batches.back();
			batch.pass = { m_offscreen, m_width, m_height };
			batch.vertices = { GL_ARRAY_BUFFER };
			batch.geometry = { true };
			batch.geometry.swizzle(batch.vertices, sizeof(vertex), { GL_FLOAT, GL_FLOAT }, { 2, 2 }, { offsetof(vertex, pos), offsetof(vertex, texture) });
			batch.texture.image2d(GL_RGBA, GL_RGBA, width, height, 0, GL_UNSIGNED_BYTE, data); // actual loading
			batch.pass.bind_texture(batch.texture);
		}

	private:
		int m_width;
		int m_height;

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

		blur<3, 2> m_blur_passes;
		gl_pass m_postprocess;
	};
}