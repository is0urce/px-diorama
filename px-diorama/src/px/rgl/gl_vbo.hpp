// name: gl_vbo
// type: c++ header
// auth: is0urce
// desc: RIAA vertex buffer object class

#pragma once

#include <GL/GL.h>
#include <vector>

namespace px
{
	class gl_vbo
	{
	public:
		operator GLuint() const noexcept
		{
			return m_vbo[0];
		}
		GLuint operator[](size_t index) const
		{
			return m_vbo[index];
		}
		void swap(gl_vbo & vbo)
		{
			std::swap(m_count, vbo.m_count);
			std::swap(m_vbo, vbo.m_vbo);
		}

		void load(size_t index, GLenum target, GLenum usage, GLsizeiptr size, void const* memory)
		{
			glBindBuffer(target, m_vbo[index]);
			glBufferData(target, size, memory, usage);
		}
		void load(GLenum target, GLenum usage, GLsizeiptr size, void const* memory)
		{
			load(0, target, usage, size, memory);
		}

	public:
		gl_vbo() noexcept
			: m_count(0)
		{
		}
		gl_vbo(GLsizei n)
			: gl_vbo()
		{
			if (n != 0)
			{
				m_count = n;
				m_vbo.assign(n, 0);
				glCreateBuffers(n, m_vbo.data());
			}
		}
		gl_vbo(bool create)
			: gl_vbo(create ? 1 : 0)
		{
		}
		gl_vbo(gl_vbo && vbo) noexcept
			: gl_vbo()
		{
			swap(vbo);
		}
		gl_vbo& operator=(gl_vbo && vbo) noexcept
		{
			swap(vbo);
			return *this;
		}
		gl_vbo(gl_vbo const&) = delete;
		gl_vbo& operator=(gl_vbo const&) = delete;
		~gl_vbo()
		{
			if (m_count != 0)
			{
				glDeleteBuffers(m_count, m_vbo.data());
			}
		}

	private:
		GLsizei m_count;
		std::vector<GLuint> m_vbo;
	};
}