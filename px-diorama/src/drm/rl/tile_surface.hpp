// name: tile_surface.hpp
// type: c++
// auth: is0urce
// desc: template class

#pragma once

#include <px/common/matrix.hpp>
#include <px/common/point.hpp>
#include <px/common/vector.hpp>
#include <px/common/stream.hpp>
#include <px/common/rectangle.hpp>

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
		rectangle bounds() const
		{
			return rectangle((m_focus - point2(Range, Range)) * point2(W, H), point2((Range * 2 + 1) * W, (Range * 2 + 1) * H));
		}
		void focus(point2 const& world_location)
		{
			point2 focus;
			std::tie(focus, std::ignore) = divide(world_location);

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
		point2 focus() const noexcept
		{
			return m_focus;
		}
		std::tuple<chunk_type *, point2> select_chunk(point2 absolute)
		{
			point2 cell;
			point2 remainder;
			std::tie(cell, remainder) = divide(absolute);

			return { chunk_by_index(cell_to_index(cell)), remainder };
		}
		std::tuple<chunk_type const*, point2> select_chunk(point2 absolute) const
		{
			point2 cell;
			point2 remainder;
			std::tie(cell, remainder) = divide(absolute);

			return { chunk_by_index(cell_to_index(cell)), remainder };
		}
		chunk_type * get_chunk(point2 cell)
		{
			return chunk_by_index(cell_to_index(cell));
		}
		chunk_type const* get_chunk(point2 cell) const
		{
			return chunk_by_index(cell_to_index(cell));
		}

		tile_type const& get_or(point2 absolute, tile_type const& or_else) const
		{
			chunk_type const* chunk;
			point2 mod;
			std::tie(chunk, mod) = select_chunk(absolute);

			return chunk ? (*chunk)[mod] : or_else;
		}

		void clear()
		{
			m_terrain.enumerate([&](point2 const& /* relative_cell */, stream_ptr & stream) {
				stream->reset();
			});
		}
		template <typename Operator>
		void load(Operator && fn)
		{
			m_terrain.enumerate([&](point2 const& index, stream_ptr & stream) {
				if (!stream) {
					stream = std::make_unique<stream_type>();
					fn(index_to_cell(index), *stream);
				}
			});
		}
		void wait()
		{
			m_terrain.enumerate([&](point2 const& /* relative_cell */, stream_ptr & stream) {
				if (stream) {
					stream->wait();
				}
			});
		}

		template <typename Operator>
		void enumerate(Operator && fn) const
		{
			m_terrain.enumerate([&](point2 const& index, stream_ptr const& stream) {
				if (stream && stream->loaded()) {
					fn(index_to_cell(index), *(stream->get()));
				}
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
		static std::tuple<point2, point2> divide(point2 a)
		{
			return divide(a, point2(W, H));
		}

		point2 cell_to_index(point2 cell) const
		{
			cell -= m_focus;
			cell += point2(Range, Range);
			return cell;
		}
		point2 index_to_cell(point2 index) const
		{
			index += m_focus;
			index -= point2(Range, Range);
			return index;
		}
		chunk_type const* chunk_by_index(point2 index)
		{
			stream_type * stream = m_terrain.contains(index) ? m_terrain[index].get() : nullptr;
			chunk_type * chunk = (stream && stream->loaded()) ? stream->get() : nullptr;
			return chunk;
		}
		chunk_type const* chunk_by_index(point2 index) const
		{
			stream_type const* stream = m_terrain.contains(index) ? m_terrain[index].get() : nullptr;
			chunk_type const* chunk = (stream && stream->loaded()) ? stream->get() : nullptr;
			return chunk;
		}

	private:
		sheets_type	m_terrain;
		point2		m_focus;
	};
}
