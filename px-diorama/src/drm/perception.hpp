// name: perception.hpp
// type: c++
// auth: is0urce
// desc: data for rendering

#pragma once

#include "vertex.hpp"

#include <vector>

namespace px
{
	class perception
	{
	public:
		void add_texture()
		{
			m_batches.emplace_back();
		}
		float scale() const noexcept
		{
			return m_scale;
		}
		void scale(float factor)
		{
			m_scale *= factor + 1.0f;
			m_scale = std::min(10000.0f, std::max(m_scale, 0.0001f)); // clamp
		}

		std::vector<std::vector<vertex>> & batches() noexcept
		{
			return m_batches;
		}
		std::vector<vertex> const& batch(size_t n) const noexcept
		{
			return m_batches[n];
		}
		size_t textures() const noexcept
		{
			return m_batches.size();
		}
		void clear()
		{
			for (auto & b : m_batches)
			{
				b.resize(0);
			}
		}

	public:
		perception()
			: m_scale(1.0)
		{
		}

	private:
		std::vector<std::vector<vertex>> m_batches;
		float m_scale;
	};
}