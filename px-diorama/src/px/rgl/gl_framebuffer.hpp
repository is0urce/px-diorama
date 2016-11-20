// name: gl_framebuffer
// type: c++ header
// auth: is0urce
// desc: RIAA framebuffer class

#pragma once

#include <GL/GL.h>

namespace px
{
	class gl_framebuffer
	{
	public:
		void swap(gl_framebuffer & that) noexcept
		{
			std::swap(m_init, that.m_init);
			std::swap(m_framebuffer, that.m_framebuffer);
		}
		operator GLuint() const noexcept
		{
			return m_framebuffer;
		}
		void init()
		{
			if (!m_init)
			{
				glGenFramebuffers(1, &m_framebuffer);
				m_init = true;
			}
		}
		void release()
		{
			if (m_init)
			{
				glDeleteFramebuffers(1, &m_framebuffer);
				m_init = false;
			}
		}
		void texture(GLuint texture, int attach)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
			glBindTexture(GL_TEXTURE_2D, texture);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attach, GL_TEXTURE_2D, texture, 0);
		}

	public:
		gl_framebuffer() noexcept
			: m_framebuffer(0)
			, m_init(false)
		{
		}
		gl_framebuffer(bool create) noexcept
			: gl_framebuffer()
		{
			if (create)
			{
				init();
			}
		}
		gl_framebuffer(gl_framebuffer && that) noexcept
			: gl_framebuffer()
		{
			swap(that);
		}
		gl_framebuffer& operator=(gl_framebuffer && that) noexcept
		{
			swap(that);
			return *this;
		}
		gl_framebuffer(gl_framebuffer const&) = delete;
		gl_framebuffer& operator=(gl_framebuffer const&) = delete;
		~gl_framebuffer()
		{
			release();
		}

	private:
		bool m_init;
		GLuint m_framebuffer;
	};
}