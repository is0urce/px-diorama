// name: renderer.hpp
// type: c++
// auth: is0urce
// desc: rendering procedures

#define GLM_FORCE_RADIANS
#pragma warning(push)	// disable for this header only & restore original warning level
#pragma warning(disable:4201) // unnamed unions for rgba and xyzw in glm library
#include <glm/glm.hpp>
#pragma warning(pop)

#include <px/common/logger.hpp>
#include <px/rgl/gl_shader.hpp>
#include <px/rgl/gl_program.hpp>
#include <px/rgl/gl_buffer.hpp>
#include <px/rgl/gl_vao.hpp>
#include <px/rgl/gl_texture.hpp>
#include <px/rgl/gl_framebuffer.hpp>

#include <array>
#include <stdexcept>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

namespace px
{
	std::string read_file(std::string name)
	{
		std::ifstream stream;
		stream.open(name);

		std::stringstream ss;
		ss << stream.rdbuf();
		return ss.str();
	}

	static const struct vertex
	{
		glm::vec2 pos;
		glm::vec3 color;
	} g_vertices[3]{
		{ { -0.6f, -0.4f },{ 1.f, 1.f, 0.f } },
		{ { 0.6f, -0.4f },{ 0.f, 1.f, 1.f } },
		{ { 0.f,  0.6f },{ 1.f, 0.f, 1.f } }
	};

	class renderer
	{
	public:
		void render()
		{
#if _DEBUG
			GLenum err = GL_NO_ERROR;
			while ((err = glGetError()) != GL_NO_ERROR)
			{
				throw std::runtime_error("OpenGL error #" + std::to_string(err) + reinterpret_cast<char const*>(glewGetErrorString(err)));
			}
#endif
			// prepare data
			m_camera.data = { { 1.2f, 1.2f },{ 0.0, 0.0 } };
			m_camera.block.load(GL_STREAM_DRAW, m_camera.data);

			// g-pass
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_offscreen);
			glViewport(0, 0, m_width, m_height);
			glUseProgram(m_batch);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_camera.block);
			glBindVertexArray(m_geometry);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			// postprocess
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			//glViewport(0, 0, m_width, m_height);
			//glUseProgram(m_process);
			//glActiveTexture(GL_TEXTURE0 + 0);
			//glBindTexture(GL_TEXTURE_2D, m_colors);
			//glDrawArrays(GL_QUADS, 0, 4);


			m_blur_sampling.data.direction = { 1.0 / m_width, 1.0 / m_height };
			m_blur_sampling.block.load(GL_STATIC_DRAW, m_blur_sampling.data);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glViewport(0, 0, m_width, m_height);
			glUseProgram(m_blur);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_blur_sampling.block);
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, m_colors);
			glDrawArrays(GL_QUADS, 0, 4);
		}
		void resize(int width, int height)
		{
			m_width = width;
			m_height = height;
		}

	public:
		renderer(int width, int height)
			: m_width(width), m_height(height)
			, m_offscreen{ true }
			, m_vertices{ GL_ARRAY_BUFFER }
			, m_geometry{ true }
			, m_batch{ { GL_VERTEX_SHADER, read_file("data/shaders/strip.vert").c_str() }, { GL_FRAGMENT_SHADER, read_file("data/shaders/strip.frag").c_str() } }
			, m_process{ { GL_VERTEX_SHADER, read_file("data/shaders/process.vert").c_str() }, { GL_FRAGMENT_SHADER, read_file("data/shaders/process.frag").c_str() } }
			, m_blur{ { GL_VERTEX_SHADER, read_file("data/shaders/blur.vert").c_str() },{ GL_FRAGMENT_SHADER, read_file("data/shaders/blur.frag").c_str() } }
		{
			m_camera.block = gl_buffer{ GL_UNIFORM_BUFFER };
			m_blur_sampling.block = gl_buffer{ GL_UNIFORM_BUFFER };

			m_batch.uniform_block("Camera", 0);
			m_process.uniform("img", 0);

			m_blur.uniform_block("BlurUniform", 0);
			m_blur.uniform("img", 0);

			m_colors.image2d(GL_RGBA32F, GL_RGBA, width, height);
			m_geometry.swizzle(m_vertices, sizeof(vertex), { GL_FLOAT, GL_FLOAT }, { 2, 3 }, { offsetof(vertex, pos), offsetof(vertex, color) });
			m_offscreen.texture(m_colors, 0);

			// static data
			m_vertices.load(GL_STREAM_DRAW, sizeof(g_vertices), g_vertices);

			m_blur_sampling.data.multipliers[0].value0 = 1.0f / 16.0f;
			m_blur_sampling.data.multipliers[1].value0 = 4.0f / 16.0f;
			m_blur_sampling.data.multipliers[2].value0 = 6.0f / 16.0f;
			m_blur_sampling.data.multipliers[3].value0 = 4.0f / 16.0f;
			m_blur_sampling.data.multipliers[4].value0 = 1.0f / 16.0f;
		}

	private:
		int m_width;
		int m_height;

		gl_texture m_colors;
		gl_framebuffer m_offscreen;

		gl_buffer m_vertices;
		gl_vao m_geometry;
		gl_program m_batch;
		gl_program m_process;
		gl_program m_blur;

		struct
		{
			gl_buffer block;
			struct
			{
				glm::vec2 scale;
				glm::vec2 offset;
			} data;
		} m_camera;

		struct
		{
			gl_buffer block;
			struct
			{
				struct entry
				{
					float value0;
					glm::vec3 padding0; // std140 requires vec4 alignments
				};
				glm::vec2 direction;
				glm::vec2 padding0;
				std::array<entry, 5> multipliers;
			} data;
		} m_blur_sampling;

	};
}