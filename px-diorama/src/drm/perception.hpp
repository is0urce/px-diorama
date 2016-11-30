// name: perception.hpp
// type: c++
// auth: is0urce
// desc: data for rendering

#pragma once

#define GLM_FORCE_RADIANS
#pragma warning(push)
#pragma warning(disable:4201) // warning C4201: nonstandard extension used: nameless struct/union
#include <glm/glm.hpp>
#pragma warning(pop)

#include <vector>

namespace px
{
	struct vertex
	{
		glm::vec2 pos;
		glm::vec2 texture;
	};

	class perception
	{
	public:
		void add_texture()
		{
			m_batches.emplace_back();
		}
		std::vector<vertex> & batch(size_t n) noexcept
		{
			return m_batches[n];
		}
		std::vector<vertex> const& batch(size_t n) const noexcept
		{
			return m_batches[n];
		}
		size_t batches() const noexcept
		{
			return m_batches.size();
		}

	public:
		perception()
		{
		}

	private:
		std::vector<std::vector<vertex>> m_batches;
	};
}