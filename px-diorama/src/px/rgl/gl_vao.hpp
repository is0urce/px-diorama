// name: gl_vao
// type: c++ header
// auth: is0urce
// desc: RIAA vertex attribute object

#pragma once

#include <GL/GL.h>

namespace px {

	class gl_vao
	{
	public:
		operator GLuint() const noexcept
		{
			return m_vao;
		}
		void swap(gl_vao & vao)
		{
			std::swap(m_init, vao.m_init);
			std::swap(m_vao, vao.m_vao);
		}
		void enable(GLuint index)
		{
			glEnableVertexArrayAttrib(m_vao, index);
		}
		void disable(GLuint index)
		{
			glDisableVertexArrayAttrib(m_vao, index);
		}
		void swizzle(GLuint vertex_buffer, GLsizei stride, std::vector<GLenum> const& types, std::vector<GLint> const& sizes, std::vector<size_t> const& offsets)
		{
			init();
			glBindVertexArray(m_vao);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
			for (size_t i = 0, size = types.size(); i != size; ++i)
			{
				glEnableVertexAttribArray(static_cast<GLuint>(i));
				glVertexAttribPointer(static_cast<GLuint>(i), sizes[i], types[i], GL_FALSE, stride, reinterpret_cast<char const*>(0) + offsets[i]);
			}
		}
		void init()
		{
			if (!m_init)
			{
				glGenVertexArrays(1, &m_vao);
				m_init = true;
			}
		}
		void release()
		{
			if (m_init)
			{
				glDeleteVertexArrays(1, &m_vao);
				m_vao = 0;
				m_init = false;
			}
		}

	public:
		gl_vao() noexcept
			: m_init(false)
			, m_vao(0)
		{
		}
		gl_vao(bool create)
			: gl_vao()
		{
			if (create)
			{
				init();
			}
		}
		gl_vao(gl_vao && vao) noexcept
			: gl_vao()
		{
			swap(vao);
		}
		gl_vao& operator=(gl_vao && vao) noexcept
		{
			swap(vao);
			return *this;
		}
		gl_vao(gl_vao const&) = delete;
		gl_vao& operator=(gl_vao const&) = delete;
		~gl_vao()
		{
			release();
		}

	private:
		bool m_init;
		GLuint m_vao;
	};
}