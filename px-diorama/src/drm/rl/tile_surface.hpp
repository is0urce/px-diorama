// name: tile_surface.hpp
// type: c++
// auth: is0urce
// desc: template class

#pragma once

#include <px/common/matrix.hpp>
#include <px/common/point.hpp>
#include <px/common/vector.hpp>
#include <px/common/stream.hpp>

#include <memory>
#include <tuple>

namespace px {

	template <typename Tile, size_t W, size_t H, size_t Range>
	class tile_surface
	{
	public:
		typedef Tile tile_type;
		typedef matrix2<tile_type, W, H> chunk_type;
		typedef stream<chunk_type> stream_type;
		typedef std::unique_ptr<stream_type> stream_ptr;
		typedef matrix2<stream_ptr, Range * 2 + 1, Range * 2 + 1> sheets_type;

	public:
		void focus(point2 const& world_location)
		{
			point2 focus;
			std::tie(focus, std::ignore) = divide(world_location, point2(W, H));

			// shift maps
			if (m_focus != focus) {
				sheets_type origin;
				origin.swap(m_terrain);

				// copy old
				point2 shift = focus - m_focus;
				m_terrain.enumerate([&](point2 index, auto & stream_ptr) {
					index += shift;

					if (origin.contains(index)) {
						std::swap(stream_ptr, origin[index]);
					}
				});

				m_focus = focus;
			}
		}
		std::tuple<chunk_type *, point2> get_chunk(point2 const& absolute)
		{
			point2 div;
			point2 mod;
			std::tie(div, mod) = translate(absolute);

			stream_type * stream = m_terrain.contains(div) ? m_terrain[div].get() : nullptr;
			chunk_type * chunk = (stream && stream->loaded()) ? stream->get() : nullptr;
			return { chunk, mod };
		}
		std::tuple<chunk_type const*, point2> get_chunk(point2 const& absolute) const
		{
			point2 div;
			point2 mod;
			std::tie(div, mod) = translate(absolute);

			stream_type const* stream = m_terrain.contains(div) ? m_terrain[div].get() : nullptr;
			chunk_type const* chunk = (stream && stream->loaded()) ? stream->get() : nullptr;
			return { chunk, mod };
		}

		tile_type const& get_or(point2 const& absolute, tile_type const& or_else) const
		{
			chunk_type const* chunk;
			point2 mod;
			std::tie(chunk, mod) = get_chunk(absolute);

			return chunk ? (*chunk)[mod] : or_else;
		}

		void clear()
		{
			m_terrain.enumerate([&](point2 const& /* relative_cell */, stream_ptr & stream) {
				stream.release();
			});
		}
		template <typename Operator>
		void load(Operator && fn)
		{
			m_terrain.enumerate([&](point2 const& relative_cell, stream_ptr & stream) {
				if (!stream) {
					stream = std::make_unique<stream_type>();
					fn(relative_cell + m_focus - point2(Range, Range), *stream);
				}
			});
		}
		void wait()
		{
			m_terrain.enumerate([&](point2 const& /* relative_cell */, stream_ptr & stream) {
				if (stream) stream->wait();
			});
		}

	public:
		tile_surface()
			: m_focus(0, 0)
		{
		}

	private:
		static std::tuple<point2, point2> divide(point2 a, point2 b)
		{
			point2 div = (vector2(a) / b).floor();
			point2 mod = a - div * b;
			return { div, mod };
		}
		std::tuple<point2, point2> translate(point2 a) const
		{
			point2 relative;
			std::tie(a, relative) = divide(a, point2(W, H));
			a += point2(Range, Range);
			a -= m_focus;
			return { a, relative };
		}

	private:
		sheets_type	m_terrain;
		point2		m_focus;
	};
}
