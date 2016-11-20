// name: gl_program
// type: c++ header
// auth: is0urce
// desc: RIAA program class

#pragma once

#include <GL/GL.h>

#include "gl_shader.hpp"

#include <vector>

namespace px
{
	class gl_program
	{
	public:
		void swap(gl_program & program)
		{
			std::swap(m_init, program.m_init);
			std::swap(m_program, program.m_program);
		}
		operator GLuint() const noexcept
		{
			return m_program;
		}
		void uniform_block(GLuint index, GLuint binding)
		{
			glUniformBlockBinding(m_program, index, binding);
		}
		void uniform_block(GLchar const* name, GLuint binding)
		{
			uniform_block(glGetUniformBlockIndex(m_program, name), binding);
		}

	public:
		gl_program() noexcept
			: m_program(0)
			, m_init(false)
		{
		}
		gl_program(std::vector<GLuint> const& shaders) noexcept
			: gl_program()
		{
			m_program = glCreateProgram();
			m_init = true;

			for (auto const& shader : shaders)
			{
				glAttachShader(m_program, shader);
			}
			glLinkProgram(m_program);
			for (auto const& shader : shaders)
			{
				glDetachShader(m_program, shader);
			}
		}
		gl_program(GLuint vetrex, GLuint fragment) noexcept
			: gl_program(std::vector<GLuint>{ vetrex, fragment })
		{
		}
		gl_program(gl_shader const& vetrex, gl_shader const& fragment) noexcept
			: gl_program(std::vector<GLuint>{ vetrex, fragment })
		{
		}
		gl_program(gl_program && program) : gl_program()
		{
			swap(program);
		}
		gl_program& operator=(gl_program && program) noexcept
		{
			swap(program);
			return *this;
		}
		gl_program(gl_program const&) = delete;
		gl_program& operator=(gl_program const&) = delete;
		~gl_program()
		{
			if (m_init)
			{
				glDeleteProgram(m_program);
			}
		}

	private:
		bool m_init;
		GLuint m_program;
	};
}