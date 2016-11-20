// name: gl_texture
// type: c++ header
// auth: is0urce
// desc: RIAA texture class

#pragma once

#include <GL/GL.h>

namespace px
{
	class gl_texture
	{
	public:
		void swap(gl_texture & that) noexcept
		{
			std::swap(m_init, that.m_init);
			std::swap(m_texture, that.m_texture);
		}
		operator GLuint() const noexcept
		{
			return m_texture;
		}
		void image2d(GLenum internal_format, GLenum format, GLenum data_type, GLsizei width, GLsizei height, int mipmap, void const* data)
		{
			init();
			glBindTexture(GL_TEXTURE_2D, m_texture);
			glTexImage2D(GL_TEXTURE_2D, mipmap, internal_format, width, height, 0, format, data_type, data);
		}
		void init()
		{
			if (!m_init)
			{
				glGenTextures(1, &m_texture);
				glBindTexture(GL_TEXTURE_2D, m_texture);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				m_init = true;
			}
		}
		void release()
		{
			if (m_init)
			{
				glDeleteTextures(1, &m_texture);
				m_init = false;
			}
		}

	public:
		gl_texture() noexcept
			: m_texture(0)
			, m_init(false)
		{
		}
		gl_texture(bool create) noexcept
			: gl_texture()
		{
			if (create)
			{
				init();
			}
		}
		gl_texture(gl_texture && that) noexcept
			: gl_texture()
		{
			swap(that);
		}
		gl_texture& operator=(gl_texture && that) noexcept
		{
			swap(that);
			return *this;
		}
		gl_texture(gl_texture const&) = delete;
		gl_texture& operator=(gl_texture const&) = delete;
		~gl_texture()
		{
			release();
		}

	private:
		bool m_init;
		GLuint m_texture;
	};
}