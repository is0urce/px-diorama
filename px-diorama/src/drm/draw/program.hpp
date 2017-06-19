// name: program.hpp
// type: c++
// auth: is0urce
// desc: loading shader programs to renderer

#include <px/rgl/gl_shader.hpp>
#include <px/rgl/gl_program.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace px {

	inline std::string read_file(std::string const& name)
	{
		std::ifstream stream;
		stream.open(name);
		if (!stream.is_open()) throw std::runtime_error("px::read_file(" + name + ") fails");

		std::stringstream ss;
		ss << stream.rdbuf();
		return ss.str();
	}

	inline gl_program compile_program(std::string const& vertex_name, std::string const& fragment_name)
	{
		gl_shader vertex;
		gl_shader fragment;
		try
		{
			vertex = gl_shader(GL_VERTEX_SHADER, read_file(vertex_name).c_str());
		}
		catch (std::exception const& exc)
		{
			throw std::runtime_error("px::compile_program(name) in vertex shader '" + vertex_name + "' error=" + exc.what());
		}
		try
		{
			fragment = gl_shader(GL_FRAGMENT_SHADER, read_file(fragment_name).c_str());
		}
		catch (std::exception const& exc)
		{
			throw std::runtime_error("px::compile_program(name) in fragment shader '" + fragment_name + "' error=" + exc.what());
		}
		return gl_program(vertex, fragment);
	}
	inline gl_program compile_program(std::string const& name)
	{
		return compile_program(name + ".vert", name + ".frag");
	}
	inline gl_program compile_program(std::string const& name, std::vector<std::string> uniforms, std::vector<std::string> textures)
	{
		auto program = compile_program(name);
		for (size_t i = 0, size = uniforms.size(); i != size; ++i) {
			program.uniform_block(uniforms[i].c_str(), static_cast<GLuint>(i));
		}
		for (size_t i = 0, size = textures.size(); i != size; ++i) {
			program.uniform(textures[i].c_str(), static_cast<GLuint>(i));
		}
		return program;
	}
}