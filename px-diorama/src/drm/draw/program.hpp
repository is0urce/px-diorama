// name: program.hpp
// type: c++
// auth: is0urce
// desc: loading shader programs to renderer

#include <px/rgl/gl_shader.hpp>
#include <px/rgl/gl_program.hpp>

#include <stdexcept>
#include <fstream>
#include <string>
#include <sstream>

namespace px
{
	inline std::string read_file(std::string name)
	{
		std::ifstream stream;
		stream.open(name);

		std::stringstream ss;
		ss << stream.rdbuf();
		return ss.str();
	}

	inline gl_program compile_program(std::string vertex_name, std::string fragment_name)
	{
		try
		{
			return{ { GL_VERTEX_SHADER, read_file(vertex_name + ".vert").c_str() },{ GL_FRAGMENT_SHADER, read_file(fragment_name + ".frag").c_str() } };
		}
		catch (std::runtime_error & exc)
		{
			throw std::runtime_error("px::compile_program(name) in '" + vertex_name + ".vert' / '" + fragment_name + ".frag' error=" + exc.what());
		}
	}
	inline gl_program compile_program(std::string name)
	{
		return compile_program(name, name);
	}
}