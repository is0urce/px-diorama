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
			std::swap(m_variant, that.m_variant);
			std::swap(m_texture, that.m_texture);
		}
		operator GLuint() const noexcept
		{
			return m_texture;
		}
		void image2d(GLenum internal_format, GLenum format, GLsizei width, GLsizei height, int mipmap, GLenum data_type, void const* data)
		{
			init(GL_TEXTURE_2D);
			glBindTexture(m_variant, m_texture);
			glTexImage2D(m_variant, mipmap, internal_format, width, height, 0, format, data_type, data);
		}
		void image2d(GLenum internal_format, GLenum format, GLsizei width, GLsizei height, int mipmap)
		{
			image2d(internal_format, format, width, height, mipmap, GL_UNSIGNED_BYTE, nullptr);
		}
		void image2d(GLenum internal_format, GLenum format, GLsizei width, GLsizei height)
		{
			image2d(internal_format, format, width, height, 0);
		}
		void init(GLenum variant)
		{
			if (m_variant != variant)
			{
				release();
			}

			if (!m_init)
			{
				glGenTextures(1, &m_texture);
				glBindTexture(variant, m_texture);

				m_variant = variant;
				m_init = true;

				if (variant == GL_TEXTURE_2D)
				{
					filters(GL_LINEAR, GL_LINEAR);
					wrap(GL_CLAMP_TO_EDGE);
				}
			}
		}
		void filters(GLenum mag, GLenum min)
		{
			glBindTexture(m_variant, m_texture);
			glTexParameteri(m_variant, GL_TEXTURE_MAG_FILTER, mag);
			glTexParameteri(m_variant, GL_TEXTURE_MIN_FILTER, min);
		}
		void wrap(GLenum wrap)
		{
			glBindTexture(m_variant, m_texture);
			glTexParameteri(m_variant, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(m_variant, GL_TEXTURE_WRAP_T, wrap);

		}
		void release()
		{
			if (m_init)
			{
				glDeleteTextures(1, &m_texture);
				m_init = false;
				m_variant = GL_INVALID_ENUM;
				m_texture = 0;
			}
		}

	public:
		gl_texture() noexcept
			: m_texture(0)
			, m_variant(GL_INVALID_ENUM)
			, m_init(false)
		{
		}
		gl_texture(GLenum variant) noexcept
			: gl_texture()
		{
			init(variant);
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
		GLenum m_variant;
		GLuint m_texture;
	};
}