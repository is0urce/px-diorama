// name: perception.hpp
// type: c++
// auth: is0urce
// desc: data for rendering

#pragma once

#include "vertex.hpp"

#include <px/ui/canvas.hpp>

#include <vector>

namespace px {

	class perception
	{
	public:
		void add_texture()
		{
			m_batches.emplace_back();
		}

		// scaling

		float scale() const noexcept
		{
			return m_scale;
		}
		void zoom(float factor)
		{
			m_scale *= factor + 1.0f;
			m_scale = std::min(10000.0f, std::max(m_scale, 0.0001f)); // clamp
		}
		void set_scale(float multiplier)
		{
			m_scale = multiplier;
		}

		std::vector<std::vector<mesh_vertex>> & batches() noexcept
		{
			return m_batches;
		}
		std::vector<std::vector<mesh_vertex>> const& batches() const noexcept
		{
			return m_batches;
		}
		void clear_batches()
		{
			for (auto & b : m_batches)
			{
				b.resize(0);
			}
		}

		ui::canvas & canvas() noexcept
		{
			return m_canvas;
		}
		ui::canvas const& canvas() const noexcept
		{
			return m_canvas;
		}

	public:
		perception()
			: m_scale(1.0)
		{
		}

	private:
		float m_scale;
		std::vector<std::vector<mesh_vertex>> m_batches;
		ui::canvas m_canvas;
	};
}