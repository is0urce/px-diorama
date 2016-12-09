#pragma once

#include <px/common/rectangle.hpp>
#include <drm/rl/map_chunk.hpp>

#include <px/fn/room_aux.hpp>

#include <algorithm>
#include <random>
#include <vector>

namespace px { namespace fn {
		class dig_generator
		{
		public:
			struct room
			{
				rectangle bounds;
				unsigned int data;
			};

		public:
			void rasterize(matrix2<unsigned char> & map)
			{
				map.fill(0);
				for (auto & room : m_rooms)
				{
					room.bounds.enumerate([&](auto const& point) {
						if (!map.contains(point)) return;

						auto & tile = map[point];
						tile |= room.data;
					});
				}
			}
			std::vector<room> const& rooms() const noexcept
			{
				return m_rooms;
			}

			template <typename Random>
			void generate(Random & rng, int min, int max, int border, int rooms)
			{
				point2 center = map_bounds.range() / 2;
				point2 range = random_range(rng, { min, min }, { max, max });

				rectangle start(center - range / 2, range);
				m_rooms.push_back(room{ start, 1 });

				while (rooms > 0)
				{
					if (generate_room(rng, min, max, border)) --rooms;
				}
			}

			template <typename Random>
			bool generate_room(Random & rng, int min, int max, int border)
			{
				auto & start = random_item(rng, m_rooms);

				point2 current = random_point(rng, start.bounds);
				point2 direction = random_block_direction(rng);

				bool dig;
				
				// pre-dig
				dig = true;
				while (dig)
				{
					current.move(direction);
					dig &= start.bounds.contains(current);
				}

				dig = true;
				border += max;
				while (dig)
				{
					current.move(direction);
					--border;
					dig &= border > 0;
				}

				// create room

				point2 range = random_range(rng, { min, min }, { max, max });
				rectangle bounds(current - range / 2, range);

				if (std::any_of(std::begin(m_rooms), std::end(m_rooms), [inflated = bounds.inflated(border)](auto const& room) { return room.bounds.intersects(inflated); })) return false;

				m_rooms.push_back(room{ bounds, 1 });

				// actual corridor dig

				return true;
			}

		public:
			dig_generator()
				: dig_generator(0, 0)
			{
			}
			dig_generator(unsigned int width, unsigned int height)
				: map_bounds({ 0, 0 }, { static_cast<int>(width), static_cast<int>(height) })
			{
			}

		private:
			rectangle map_bounds;
			std::vector<room> m_rooms;
		};
	}
}