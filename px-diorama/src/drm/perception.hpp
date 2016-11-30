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
			auto & batch = m_batches.back();

			batch.assign(4, {});

			batch[0].pos = { -1.0f, 1.0f };
			batch[0].texture = { 0.0f, 0.0f };

			batch[1].pos = { -1.0f, -1.0f };
			batch[1].texture = { 0.0f, 1.0f };

			batch[2].pos = { 1.0f,  -1.0f };
			batch[2].texture = { 1.0f, 1.0f };

			batch[3].pos = { 1.0f,  1.0f };
			batch[3].texture = { 1.0f,  0.0f };
		}
		std::vector<vertex> const& batch(size_t n) const noexcept
		{
			return m_batches[n];
		}

	public:
		perception()
		{
		}

	private:
		std::vector<std::vector<vertex>> m_batches;
	};
}