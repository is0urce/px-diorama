// name: gl_assert
// type: c++ header
// auth: is0urce
// desc: assert funtion

#pragma once

#if _DEBUG

#include <GL/GL.h>
#include <sstream>

namespace px
{
	inline void gl_assert()
	{
		GLenum err = GL_NO_ERROR;
		if ((err = glGetError()) != GL_NO_ERROR)
		{
			std::stringstream ss;
			ss << "OpenGL error #" + std::to_string(err);
			while ((err = glGetError()) != GL_NO_ERROR)
			{
				ss << ", #" + std::to_string(err);
			}
			throw std::runtime_error(ss.str());
		}
	}
}

#else

namespace px
{
	inline void gl_assert()
	{
	}
}

#endif
