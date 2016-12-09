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
			enum class tile : unsigned char { room, corridor };
			struct room
			{
				rectangle bounds;
				tile data;
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
						tile = 1;
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
				m_rooms.push_back(room{ start, tile::room });

				while (rooms > 0)
				{
					if (generate_room(rng, min, max, border)) --rooms;
				}
			}

			template <typename Random>
			bool generate_room(Random & rng, int min, int max, int border)
			{
				// select room
				auto start = random_item_filtered(rng, m_rooms, [](auto const& room) { return room.data == tile::room; });

				// random point in room
				point2 from = random_point(rng, start.bounds);
				point2 direction = random_block_direction(rng);
				point2 normal(-direction.y(), direction.x());

				point2 current = from;
				int steps = 0;
				bool dig;
				
				// move to wall
				dig = true;
				while (dig)
				{
					current.move(direction);
					++steps;
					dig &= start.bounds.contains(current);
				}

				// exit wall range
				current.move(direction * border);

				// dig to max range
				dig = true;
				int move = max;
				while (dig)
				{
					current.move(direction);
					++steps;
					--move;
					dig &= !intersect_existing(rectangle(current, { 0, 0 }).inflated(border));
					dig &= move > 0;
				}

				// room params

				point2 range = random_range(rng, { min, min }, { max, max });
				point2 anchor = current - range / 2;

				rectangle candidate{ anchor, range };
				if (intersect_existing(candidate.inflated(border))) return false; // can't fit in any case

				// clamp to wall
				int shift = 0;
				while (!intersect_existing(candidate.inflated(border)))
				{
					++shift;
					candidate = { anchor - direction * shift, range };
				} 
				--shift;
				candidate = { anchor - direction * shift, range };

				// corridor
				point2 end = current - direction * shift;
				rectangle corridor = rectangle::from_corners(from, end + normal);

				// add
				m_rooms.push_back(room{ candidate, tile::room });
				m_rooms.push_back(room{ corridor, tile::corridor });

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
			bool intersect_existing(rectangle bounds) const
			{
				return std::any_of(std::begin(m_rooms), std::end(m_rooms), [&](auto const& room) { return room.bounds.intersects(bounds); });
			}

		private:
			rectangle map_bounds;
			std::vector<room> m_rooms;
		};
	}
}