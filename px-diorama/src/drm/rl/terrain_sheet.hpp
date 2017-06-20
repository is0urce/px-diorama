// name: tile_sheet.hpp

#pragma once

#include "terrain_stream.hpp"
#include <px/common/assert.hpp>
#include <px/common/matrix.hpp>

#include <memory>

namespace px {

	template <typename Tile, size_t W, size_t H>
	class terrain_sheet
	{
	public:
		typedef Tile tile_type;
		typedef matrix2<tile_type, W, H> chunk_type;
		typedef stream<chunk_type> stream_type;
		typedef std::unique_ptr<stream_type> stream_ptr;
		typedef matrix2<stream_ptr, 3, 3> sheet_type;

	public:
		void assign_default(tile_type tile)
		{
			m_default = tile;
		}
		tile_type const& get_or(point2 const& absolute, tile_type const& or_else) const
		{
			stream_type * stream;
			return (stream && stream->loaded()) ? (*stream).data()[absolute] : m_default
		}
		tile_type const* operator[](point2 const& absolute) const
		{
			return (*m_chunk)[absolute];
		}
		void clear()
		{
			m_terrain.fill(nullptr);
		}
		void focus(point2 const& absolute)
		{
		}

	public:
		terrain_sheet()
		{
		}

	private:
		sheet_type *	m_terrain;
		tile_type		m_default;
		point2			m_focus;
	};
}
