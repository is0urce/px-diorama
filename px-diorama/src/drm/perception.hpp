// name: perception.hpp
// type: c++
// auth: is0urce
// desc: data for rendering

// aggregator of drawing data for rendering

#pragma once

#include "vertex.hpp"

#include <px/ui/canvas.hpp>

#include <vector>

namespace px {

	class perception
	{
	public:
		typedef std::vector<std::vector<mesh_vertex>> arrays_type; // same as in sprite system

	public:
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

		// draw lists

		std::vector<std::vector<mesh_vertex>> const* batches() const noexcept
		{
			return m_vertices;
		}
		void assign_batches(std::vector<std::vector<mesh_vertex>> const* data) noexcept
		{
			m_vertices = data;
		}

		// ui grid

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
		std::vector<std::vector<mesh_vertex>> const* m_vertices;
		ui::canvas m_canvas;
		float m_scale;
	};
}