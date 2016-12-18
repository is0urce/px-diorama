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

#include <cmath>
#include <array>
#include <vector>

namespace px
{
	template <size_t Stages, size_t SampleRange>
	class blur final
	{
	private:
		struct stage;
		struct sample;
		struct uniform_block;

	public:
		void draw_arrays(GLenum e, GLsizei count)
		{
			for (size_t i = 0; i != Stages; ++i)
			{
				m_stages[i].pass.draw_arrays(e, count);
			}
		}
		void resize(int width, int height)
		{
			m_width = width;
			m_height = height;
		}
		GLuint result() const noexcept
		{
			return m_result;
		}

	public:
		blur()
		{
		}
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
				generate_distribution(m_stages[i].data.multipliers);

				// bind
				m_stages[i].block.load(GL_STATIC_DRAW, m_stages[i].data);

				// pass
				m_stages[i].pass = gl_pass(i % 2 == 0 ? framebuffer0 : framebuffer1, 0, m_width, m_height);

				m_stages[i].pass.bind_uniform(m_stages[i].block);
				m_stages[i].pass.bind_texture(i == 0 ? texture_start : ((i % 2 == 0) ? albedo1 : albedo0)); // alternating ping-pong
			}

			m_result = Stages % 2 == 0 ? albedo1 : albedo0;
		}

	private:
		// uses binomial distribution
		void generate_distribution(std::array<sample, SampleRange + 1> & multipliers)
		{
			std::array<unsigned int, SampleRange + 1> first;
			std::array<unsigned int, SampleRange> second;

			// first iteration
			first[0] = 1;

			for (int k = 0; k != SampleRange; ++k)
			{
				for (int i = 0, end = k + 1; i != end; ++i)
				{
					second[i] = first[i] + (((i + 1) == end) ? 0 : first[i + 1]);
				}
				for (int i = 0, end = k + 2; i != end; ++i)
				{
					first[i] = (i == 0 ? second[0] : second[i - 1]) + (i + 1 == end ? 0 : second[i]);
				}
			}

			auto total = std::pow(2.0f, 2 * SampleRange);
			for (size_t i = 0; i <= SampleRange; ++i)
			{
				multipliers[i].value0 = first[i] / static_cast<float>(total);
			}
		}

	private:
		int m_width;
		int m_height;
		GLuint m_result;

		struct sample
		{
			glm::float32 value0;
			glm::vec3 padding0; // std140 requires vec4 alignments
		};
		struct uniform_block
		{
			glm::vec2 direction;
			glm::float32 bokeh;
			glm::float32 padding0;
			std::array<sample, SampleRange + 1> multipliers;
		};
		struct stage
		{
			uniform_block data;
			gl_buffer block;
			gl_pass pass;
		};

		std::array<stage, Stages> m_stages;
	};
}