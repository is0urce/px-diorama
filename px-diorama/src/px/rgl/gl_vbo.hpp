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
			return m_vbo;
		}
		void swap(gl_vbo & vbo)
		{
			std::swap(m_init, vbo.m_init);
			std::swap(m_vbo, vbo.m_vbo);
		}

		void load(GLenum target, GLenum usage, GLsizeiptr size, void const* memory)
		{
			glBindBuffer(target, m_vbo);
			glBufferData(target, size, memory, usage);
		}

		template <typename Structure>
		void load(GLenum target, GLenum usage, Structure const& pod)
		{
			load(target, usage, sizeof(&pod), &pod);
		}

	public:
		gl_vbo() noexcept
			: m_init(false)
		{
		}
		gl_vbo(bool create)
			: gl_vbo()
		{
			if (create)
			{
				glCreateBuffers(1, &m_vbo);
				m_init = true;
			}
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
			if (m_init)
			{
				glDeleteBuffers(1, &m_vbo);
			}
		}

	private:
		bool m_init;
		GLuint m_vbo;
	};
}