// name: gl_pass
// type: c++ header
// auth: is0urce
// desc: renderpass state mashine abstraction

#pragma once

#include <GL/GL.h>

#include <vector>

namespace px
{
	class gl_pass
	{
	public:
		struct attachment
		{
			GLuint binding;
			GLuint element;
		};

	public:
		void swap(gl_pass & that) noexcept
		{
			std::swap(m_init, that.m_init);
			std::swap(m_framebuffer, that.m_framebuffer);
			std::swap(m_width, that.m_width);
			std::swap(m_height, that.m_height);
			std::swap(m_textures, that.m_textures);
			std::swap(m_uniforms, that.m_uniforms);
		}
		void viewport(GLsizei width, GLsizei height) noexcept
		{
			m_width = width;
			m_height = height;
		}
		void texture(GLuint texture, GLuint index)
		{
			m_textures.push_back({ index, texture });
		}
		void texture(GLuint texture)
		{
			m_textures.push_back({ 0, texture });
		}
		void uniform(GLuint ubo, GLuint index)
		{
			m_uniforms.push_back({ index, ubo });
		}
		void uniform(GLuint ubo)
		{
			m_uniforms.push_back({ 0, ubo });
		}

	public:
		void bind()
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebuffer);
			glViewport(0, 0, m_width, m_height);
			for (size_t i = 0, size = m_uniforms.size(); i != size; ++i)
			{
				glBindBufferBase(GL_UNIFORM_BUFFER, m_uniforms[i].binding, m_uniforms[i].element);
			}
			for (size_t i = 0, size = m_textures.size(); i != size; ++i)
			{
				glActiveTexture(GL_TEXTURE0 + m_textures[i].binding);
				glBindTexture(GL_TEXTURE_2D, m_textures[i].element);
			}
		}
		void draw(GLenum mode, GLsizei count, GLint first)
		{
			bind();
			glDrawArrays(mode, first, count);
		}
		void draw(GLenum mode, GLsizei count)
		{
			draw(mode, count, 0);
		}

	public:
		gl_pass() noexcept
			: m_framebuffer(0)
			, m_init(false)
			, m_width(0)
			, m_height(0)
		{
		}
		gl_pass(GLuint framebuffer, GLsizei width, GLsizei height) noexcept
			: m_framebuffer(framebuffer)
			, m_init(true)
		{
			viewport(width, height);
		}
		gl_pass(GLuint framebuffer) noexcept
			: gl_pass(framebuffer, 0, 0)
		{
		}
		gl_pass(gl_pass const&) = delete;
		gl_pass& operator=(gl_pass const&) = delete;
		gl_pass(gl_pass && that) noexcept
			: gl_pass()
		{
			swap(that);
		}
		gl_pass& operator=(gl_pass && that) noexcept
		{
			swap(that);
			return *this;
		}
		~gl_pass()
		{
		}

	private:
		bool m_init;
		GLuint m_framebuffer;
		GLsizei m_width;
		GLsizei m_height;
		std::vector<attachment> m_textures;
		std::vector<attachment> m_uniforms;
	};
}