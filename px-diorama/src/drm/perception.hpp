// name: perception.hpp
// type: c++
// auth: is0urce
// desc: data for rendering

// aggregator of drawing data for rendering

#pragma once

#include "draw/popup.hpp"
#include "vertex.hpp"

#include <px/ui/canvas.hpp>

#include <vector>

namespace px {

	class perception
	{
	public:
		typedef std::vector<std::vector<mesh_vertex>> arrays_type; // same as in sprite system
		typedef std::vector<popup> popups_type;

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

		arrays_type const* batches() const noexcept
		{
			return m_vertices;
		}
		void assign_batches(arrays_type const* data) noexcept
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

		// popups
		void clear_popups()
		{
			m_popups.clear();
		}
		void emplace_popup(float x, float y, std::string text, color tint)
		{
			m_popups.push_back({ x, y, text, tint });
		}
		popups_type const& popups() const noexcept
		{
			return m_popups;
		}

	public:
		perception()
			: m_scale(1.0)
		{
		}

	private:
		arrays_type const* m_vertices;
		ui::canvas m_canvas;
		float m_scale;
		popups_type m_popups;
	};
}