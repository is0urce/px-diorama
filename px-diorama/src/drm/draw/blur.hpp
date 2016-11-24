// name: blur.hpp
// type: c++
// auth: is0urce
// desc: blur pass

#include <GL/GL.h>

#include <px/rgl/gl_program.hpp>
#include <px/rgl/gl_buffer.hpp>
#include <px/rgl/gl_vao.hpp>
#include <px/rgl/gl_texture.hpp>
#include <px/rgl/gl_framebuffer.hpp>
#include <px/rgl/gl_pass.hpp>

#include <array>
#include <vector>

namespace px
{
	template <size_t Stages, size_t SampleRange>
	class blur
	{
	public:
		void draw(GLuint program)
		{
			glUseProgram(program);
			m_blur_a.pass.draw_arrays(GL_QUADS, 4);
			m_blur_b.pass.draw_arrays(GL_QUADS, 4);
			m_blur_c.pass.draw_arrays(GL_QUADS, 4);
		}
		void resize(int width, int height)
		{
			m_width = width;
			m_height = height;
		}

	public:
		blur(
			GLuint texture_start,
			GLuint framebuffer0, GLuint albedo0,
			GLuint framebuffer1, GLuint albedo1,
			int width, int height,
			float bokeh,
			std::array<float, Stages> const& angles
			)
			: m_width(width), m_height(height)
		{
			auto dx = 1.0f / m_width; // one pixel
			auto dy = 1.0f / m_height;
			for (size_t i = 0; i != Stages; ++i)
			{
				// uniform
				m_stages[i].block = { GL_UNIFORM_BUFFER };

				// data
				m_stages[i].data.direction = { dx * std::cos(angles[i]), dy * std::sin(angles[i]) }; // 0 degs
				m_stages[i].data.bokeh = bokeh;
				for (size_t sample = 0; sample <= SampleRange; ++sample)
				{
					m_stages[i].data.multipliers[sample].value0 = 1.0f;
				}

				// bind
				m_stages[i].block.load(GL_STATIC_DRAW, m_stages[i].uniform.data);

				// pass
				m_stages[i].pass = gl_pass(i % 2 == 0 ? framebuffer0 : framebuffer1, m_width, m_height);

				m_stages[i].pass.bind_uniform(m_blur_a.uniform.block);
				m_stages[i].pass.bind_texture(i == 0 ? texture_start : ((i % 2 == 0) ? albedo1 : albedo0)); // alternating ping-pong
			}
		}

	private:
		int m_width;
		int m_height;

		struct uniform_data
		{
			struct entry
			{
				glm::float32 value0;
				glm::vec3 padding0; // std140 requires vec4 alignments
			};
			glm::vec2 direction;
			glm::float32 bokeh;
			glm::float32 padding0;
			std::array<entry, SampleRange + 1> multipliers;
		};
		struct stage
		{
			uniform_data data;
			gl_buffer block;
			gl_pass pass;
		};

		std::array<stage, Stages> m_passes;
	};
}