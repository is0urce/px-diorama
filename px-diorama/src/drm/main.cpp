// name: main.cpp
// type: c++
// auth: is0urce
// desc: entry point

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#pragma warning(push)	// disable for this header only & restore original warning level
#pragma warning(disable:4201) // unions for rgba and xyzw in glm library
#include <glm/glm.hpp>
#pragma warning(pop)

#include <px/common/logger.hpp>
#include <px/rgl/gl_shader.hpp>
#include <px/rgl/gl_program.hpp>
#include <px/rgl/gl_vbo.hpp>
#include <px/rgl/gl_vao.hpp>

#include <stdexcept>
#include <string>

static void error_callback(int error, const char* description)
{
	throw std::runtime_error(std::string("glfw error code = " + std::to_string(error) + " " + std::string(description)));
}
static void key_callback(GLFWwindow* window, int key, int /* scancode */, int action, int /* mods */)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// RAII glfw init and termination
// (plus error handling)
class glfw_init
{
public:
	glfw_init()
		: m_init(false)
	{
		glfwSetErrorCallback(&error_callback);
		m_init = glfwInit() != 0;
	}
	~glfw_init()
	{
		if (m_init)
		{
			glfwTerminate();
		}
	}
	bool success() const noexcept
	{
		return m_init;
	}
private:
	bool m_init;
};

static const struct vertex
{
	glm::vec2 pos;
	glm::vec3 color;
} vertices[3] {
	{ {-0.6f, -0.4f}, {1.f, 0.f, 0.f} },
	{ {0.6f, -0.4f}, {0.f, 1.f, 0.f} },
	{ {0.f,  0.6f}, {0.f, 0.f, 1.f} }
};

// application starts here
int main()
{
	px::logger logger;
	try
	{
		glfw_init init;

		// create window and contest
		auto *window = glfwCreateWindow(800, 600, "prss-x-drm", nullptr, nullptr);
		glfwSetKeyCallback(window, key_callback);
		glfwMakeContextCurrent(window);
		glewInit();	// initialize extensions wrangler (need context first)

		px::gl_program program({
			px::gl_shader(GL_VERTEX_SHADER,
				"#version 330\n"
				"#extension GL_ARB_separate_shader_objects : enable\n"
				"attribute layout(location = 0) in vec2 inPos;\n"
				"attribute layout(location = 1) in vec3 inColor;\n"
				"layout(location = 0) out vec3 outColor;\n"
				"void main()\n"
				"{\n"
				"    gl_Position = vec4(inPos, 0.0, 1.0);\n"
				"    outColor = inColor;\n"
				"}\n"),
			px::gl_shader(GL_FRAGMENT_SHADER,
				"#version 330\n"
				"#extension GL_ARB_separate_shader_objects : enable\n"
				"layout(location = 0) in vec3 color;\n"
				"layout(location = 0) out vec4 fragColor;\n"
				"void main()\n"
				"{\n"
				"    fragColor = vec4(color, 1.0);\n"
				"}\n") });


		px::gl_vbo vbo(true);
		px::gl_vao vao(true);
		vao.swizzle(vbo, sizeof(vertex), { GL_FLOAT, GL_FLOAT }, { 2, 3 }, { offsetof(vertex,pos), offsetof(vertex,color) });

		glfwSwapInterval(1); // vsync
		// main loop
		while (!glfwWindowShouldClose(window))
		{
#if _DEBUG
			GLenum err = GL_NO_ERROR;
			while ((err = glGetError()) != GL_NO_ERROR)
			{
				throw std::runtime_error("renderer::render(..) - OpenGL error");
			}
#endif
			// prepare data
			vbo.load(GL_ARRAY_BUFFER, GL_STREAM_DRAW, sizeof(vertices), vertices);

			// render
			glClear(GL_COLOR_BUFFER_BIT);
			glUseProgram(program);
			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			// i/o
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glfwDestroyWindow(window);
	}
	catch (std::runtime_error &exc)
	{
		logger.write(exc.what());
		return -1;
	}
	catch (...)
	{
		logger.write("unhandled system error");
		return -1;
	}

	return 0;
}