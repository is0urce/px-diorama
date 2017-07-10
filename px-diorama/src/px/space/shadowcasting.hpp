// name: shadowcasting.hpp
// type: c++ header & implementation
// auth: is0urce
// desc: recursive shadowcasting

#pragma once

#include <px/common/point.hpp>

#include <memory>
#include <functional>
#include <stdexcept>
#include <vector>

namespace px {

	namespace
	{
		// octants
		static int multipliers[4][8] =
		{
			{ 1, 0, 0, -1, -1, 0, 0, 1 },
			{ 0, 1, -1, 0, 0, -1, 1, 0 },
			{ 0, 1, 1, 0, 0, -1, -1, 0 },
			{ 1, 0, 0, 1, -1, 0, 0, -1 }
		};
	}

	class shadowcasting
	{
	public:
		typedef std::function<bool(point2 const& position)> map_fn;

		void reset()
		{
			m_fov.assign(m_width * m_width, false);
		}
		void radius(unsigned int radius_size)
		{
			m_radius = radius_size;
			m_width = m_radius * 2 + 1;
			reset();
		}
		bool in_range(point2 const& absolute) const noexcept
		{
			return contains(translate(absolute));
		}
		bool in_sight(point2 absolute) const noexcept
		{
			auto position = translate(absolute);
			return contains(position) && m_fov[position.y() * m_width + position.x()];
		}
		void light(point2 absolute)
		{
			auto position = translate(absolute);
			if (contains(position)) {
				m_fov[static_cast<size_t>(position.y() * m_width + position.x())] = true;
			}
		}

		template <typename Predicate>
		void recursive(Predicate && fn, point2 const& start)
		{
			recursive(std::forward<Predicate>(fn), start, m_radius);
		}
		template <typename Predicate>
		void recursive(Predicate && fn, point2 const& start, unsigned int radius_size)
		{
			if (radius_size > m_radius)
			{
				radius(radius_size);
			}

			m_offset = start;
			light(start);
			for (unsigned int i = 0; i != 8; ++i) // calculate every octant
			{
				recursive(fn, start.x(), start.y(), radius_size, 1, 1.0, 0.0, multipliers[0][i], multipliers[1][i], multipliers[2][i], multipliers[3][i]);
			}
		}

	public:
		shadowcasting(unsigned int radius_size)
		{
			radius(radius_size);
		}
		shadowcasting(unsigned int radius_size, point2 const& start, map_fn && predicate)
			: shadowcasting(radius_size)
		{
			recursive(std::forward<map_fn>(predicate), start, radius_size);
		}

	private:
		bool contains(point2 const& position) const noexcept
		{
			return position.x() >= 0 && position.y() >= 0 && position.x() < static_cast<int>(m_width) && position.y() < static_cast<int>(m_width);
		}
		point2 translate(point2 absolute) const noexcept
		{
			return absolute + point2(m_radius, m_radius) - m_offset;
		}
		template <typename Predicate>
		void recursive(Predicate && predicate, int x, int y, unsigned int radius_size, unsigned int row, float start, float end, int xx, int xy, int yx, int yy)
		{
			if (start < end) return;
			int radius2 = static_cast<int>(radius_size * radius_size);

			float next_start_slope = start;
			for (unsigned int i = row; i <= radius_size; ++i)
			{
				int dx = 0 - i - 1;
				int dy = 0 - i;
				bool blocked = false;
				while (dx <= 0)
				{
					++dx;
					float l_slope = (dx - 0.5f) / (dy + 0.5f);
					float r_slope = (dx + 0.5f) / (dy - 0.5f);

					if (start < r_slope)
					{
						continue;
					}
					else if (end > l_slope)
					{
						break;
					}
					else
					{
						point2 absolute(x + dx * xx + dy * xy, y + dx * yx + dy * yy);

						// to avoid behind-collumn peek you have to see center of a tile to actualy see it (or it shoult be wall)
						float slope = static_cast<float>(dx) / static_cast<float>(dy);
						bool wall = !predicate(absolute);
						bool center = start >= slope && slope >= end;
						if ((center || wall) && (dx * dx + dy * dy < radius2))
						{
							light(absolute);
						}

						if (blocked)
						{
							if (wall)
							{
								next_start_slope = r_slope;
								continue;
							}
							else
							{
								blocked = false;
								start = next_start_slope;
							}
						}
						else
						{
							if (wall && i < radius_size)
							{
								blocked = true;
								recursive(predicate, x, y, radius_size, i + 1, start, l_slope, xx, xy, yx, yy);
								next_start_slope = r_slope;
							}
						}
					}
				}
				if (blocked) break;
			}
		}

	private:
		std::vector<bool> m_fov;
		unsigned int m_radius;
		unsigned int m_width;
		point2 m_offset;
	};
}