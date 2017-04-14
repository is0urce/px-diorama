// name: gl_vbo
// type: c++ header
// auth: is0urce
// desc: RIAA vertex buffer object class

#pragma once

#include <GL/GL.h>

namespace px {

	class gl_buffer
	{
	public:
		operator GLuint() const noexcept
		{
			return m_vbo;
		}
		void swap(gl_buffer & that) noexcept
		{
			std::swap(m_init, that.m_init);
			std::swap(m_variant, that.m_variant);
			std::swap(m_vbo, that.m_vbo);
		}
		GLsizei size() const noexcept
		{
			return m_size;
		}

		void load(GLenum usage, GLsizeiptr size, void const* memory)
		{
			m_size = static_cast<GLsizei>(size);
			glBindBuffer(m_variant, m_vbo);
			glBufferData(m_variant, size, memory, usage);
		}

		template <typename Structure>
		void load(GLenum usage, Structure const& pod)
		{
			load(usage, sizeof(pod), &pod);
		}
	private:
		void init(GLenum variant)
		{
			if (m_variant != variant)
			{
				release();
				glCreateBuffers(1, &m_vbo);
				m_variant = variant;
				m_init = true;
			}
		}
		void release()
		{
			if (m_init)
			{
				glDeleteBuffers(1, &m_vbo);
				m_init = false;
				m_variant = GL_INVALID_ENUM;
			}
		}

	public:
		gl_buffer() noexcept
			: m_init(false)
			, m_variant(GL_INVALID_ENUM)
			, m_vbo(0)
			, m_size(0)
		{
		}
		gl_buffer(GLenum variant)
			: gl_buffer()
		{
			init(variant);
		}
		gl_buffer(gl_buffer && that) noexcept
			: gl_buffer()
		{
			swap(that);
		}
		gl_buffer& operator=(gl_buffer && that) noexcept
		{
			swap(that);
			return *this;
		}
		gl_buffer(gl_buffer const&) = delete;
		gl_buffer& operator=(gl_buffer const&) = delete;
		~gl_buffer()
		{
			release();
		}

	private:
		bool m_init;
		GLenum m_variant;
		GLuint m_vbo;
		GLsizei m_size;
	};
}