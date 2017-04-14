// name: gl_shader
// type: c++ header
// auth: is0urce
// desc: RIAA shader class

#pragma once

#include <GL/GL.h>

#include <vector>
#include <stdexcept>

namespace px {

	class gl_shader
	{
	public:
		void swap(gl_shader & shader) noexcept
		{
			std::swap(m_init, shader.m_init);
			std::swap(m_shader, shader.m_shader);
		}
		operator GLuint() const noexcept
		{
			return m_shader;
		}

	public:
		gl_shader() noexcept
			: m_shader(0)
			, m_init(false)
		{
		}
		gl_shader(GLenum shader_variant) noexcept
			:  gl_shader()
		{
			m_shader = glCreateShader(shader_variant);
			m_init = true;
		}
		gl_shader(GLenum shader_variant, GLchar const* code)
			: gl_shader(shader_variant)
		{
			glShaderSource(m_shader, 1, &code, nullptr);
			glCompileShader(m_shader);

			GLint status;
			glGetShaderiv(m_shader, GL_COMPILE_STATUS, &status);
			if (status == GL_FALSE)
			{
				GLint len;
				glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &len);
				std::vector<GLchar> log(len + 1, 0);
				glGetShaderInfoLog(m_shader, len, NULL, &log[0]);

				throw std::runtime_error(std::string("px::gl_shader::ctor() shader error:\n") + log.data());
			}
		}
		gl_shader(gl_shader && shader) noexcept
			: gl_shader()
		{
			swap(shader);
		}
		gl_shader& operator=(gl_shader && shader) noexcept
		{
			swap(shader);
			return *this;
		}
		gl_shader(gl_shader const&) = delete;
		gl_shader& operator=(gl_shader const&) = delete;
		~gl_shader()
		{
			if (m_init)
			{
				glDeleteShader(m_shader);
			}
		}

	private:
		bool m_init;
		GLuint m_shader;
	};
}