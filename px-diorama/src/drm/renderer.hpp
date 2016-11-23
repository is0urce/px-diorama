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
#include <px/rgl/gl_vbo.hpp>
#include <px/rgl/gl_vao.hpp>
#include <px/rgl/gl_texture.hpp>
#include <px/rgl/gl_framebuffer.hpp>

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
		{ { -0.6f, -0.4f },{ 1.f, 1.f, 1.f } },
		{ { 0.6f, -0.4f },{ 1.f, 1.f, 1.f } },
		{ { 0.f,  0.6f },{ 1.f, 1.f, 1.f } }
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
			m_camera_data = { { 1.2f, 1.2f },{ 0.0, 0.0 } };
			m_vertices.load(GL_ARRAY_BUFFER, GL_STREAM_DRAW, sizeof(g_vertices), g_vertices);
			m_camera.load(GL_UNIFORM_BUFFER, GL_STREAM_DRAW, m_camera_data);

			// g-pass
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_offscreen);
			glViewport(0, 0, m_width, m_height);
			glUseProgram(m_batch);
			glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_camera);
			glBindVertexArray(m_geometry);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			// postprocess
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glViewport(0, 0, m_width, m_height);
			glUseProgram(m_process);
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
			, m_vertices{ true }
			, m_camera{ true }
			, m_geometry{ true }
			, m_batch{ { GL_VERTEX_SHADER, read_file("data/shaders/strip.vert").c_str() }, { GL_FRAGMENT_SHADER, read_file("data/shaders/strip.frag").c_str() } }
			, m_process{ { GL_VERTEX_SHADER, read_file("data/shaders/process.vert").c_str() }, { GL_FRAGMENT_SHADER, read_file("data/shaders/process.frag").c_str() } }
		{
			m_batch.uniform_block("Camera", 0);
			m_process.uniform("img", 0);

			m_colors.image2d(GL_RGBA32F, GL_RGBA, width, height);
			m_geometry.swizzle(m_vertices, sizeof(vertex), { GL_FLOAT, GL_FLOAT }, { 2, 3 }, { offsetof(vertex, pos), offsetof(vertex, color) });
			m_offscreen.texture(m_colors, 0);
		}

	private:
		int m_width;
		int m_height;

		gl_texture m_colors;
		gl_framebuffer m_offscreen;
		gl_vbo m_vertices;
		gl_vbo m_camera;
		gl_vao m_geometry;
		gl_program m_batch;
		gl_program m_process;

		struct camera_data
		{
			glm::vec2 scale;
			glm::vec2 offset;
		} m_camera_data;
	};
}