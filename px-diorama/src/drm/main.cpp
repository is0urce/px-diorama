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
#include <sstream>

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

static struct vertex
{
	glm::vec2 pos;
	glm::vec3 color;
} g_vertices[3] {
	{ {-0.6f, -0.4f}, {1.f, 0.f, 0.f} },
	{ {0.6f, -0.4f}, {0.f, 1.f, 0.f} },
	{ {0.f,  0.6f}, {0.f, 0.f, 1.f} }
};

static struct camera
{
	glm::vec2 scale;
	glm::vec2 offset;
} g_camera;

std::string read_file(std::string name)
{
	std::ifstream stream;
	stream.open(name);

	std::stringstream ss;
	ss << stream.rdbuf();
	return ss.str();
}

// application starts here
int main()
{
	try
	{
		try
		{
			glfw_init init;

			// create window and contest
			auto *window = glfwCreateWindow(800, 600, "press-x-diorama", nullptr, nullptr);
			glfwSetKeyCallback(window, key_callback);
			glfwMakeContextCurrent(window);
			glewInit();	// initialize extensions wrangler (need context first)

			px::gl_vbo vertices(true);
			px::gl_vbo camera(true);
			px::gl_vao geometry(true);
			px::gl_program program(
				px::gl_shader(GL_VERTEX_SHADER, read_file("data\\shaders\\strip.vert").c_str()),
				px::gl_shader(GL_FRAGMENT_SHADER, read_file("data\\shaders\\strip.frag").c_str()));

			geometry.swizzle(vertices, sizeof(vertex), { GL_FLOAT, GL_FLOAT }, { 2, 3 }, { offsetof(vertex,pos), offsetof(vertex,color) });
			program.uniform_block("Camera", 0);

			glfwSwapInterval(1); // vsync
			// main loop
			while (!glfwWindowShouldClose(window))
			{
#if _DEBUG
				GLenum err = GL_NO_ERROR;
				while ((err = glGetError()) != GL_NO_ERROR)
				{
					throw std::runtime_error("OpenGL error #" + std::to_string(err));
				}
#endif
				// prepare data
				g_camera = { { 0.5f, 0.5f }, { 0.6f, 0.3f } };
				vertices.load(GL_ARRAY_BUFFER, GL_STREAM_DRAW, sizeof(g_vertices), g_vertices);
				camera.load(GL_UNIFORM_BUFFER, GL_STREAM_DRAW, sizeof(g_camera), &g_camera);

				// render
				glClear(GL_COLOR_BUFFER_BIT);

				glUseProgram(program);
				glBindBufferBase(GL_UNIFORM_BUFFER, 0, camera);

				glBindVertexArray(geometry);
				glDrawArrays(GL_TRIANGLES, 0, 3);

				// i/o
				glfwSwapBuffers(window);
				glfwPollEvents();
			}

			glfwDestroyWindow(window);
		}
		catch (std::runtime_error &exc)
		{
			throw std::move(exc);
		}
		catch (...)
		{
			throw std::runtime_error("unhandled exception");
		}
	}
	catch (std::runtime_error &exc)
	{
		px::logger logger;
		logger.write(exc.what());
		return -1;
	}
	return 0;
}