// name: gl_uniform.hpp
// type: c++ header
// auth: is0urce
// desc: class

#pragma once

// uniform block

#include "gl_buffer.hpp"

namespace px {

	struct gl_uniform
	{
	private:
		gl_buffer block;
	public:
		gl_uniform()
		{
			block = { GL_UNIFORM_BUFFER };
		}
		template <typename Structure>
		void load(GLenum mode, Structure const& data)
		{
			block.load(mode, data);
		}
		operator GLuint() const noexcept
		{
			return block;
		}
	};
}