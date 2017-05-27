// name: tile_library.hpp

#pragma once

#include <bitset>
#include <map>

namespace px {

	// where Props is tile_prototype
	template <typename TilePrototype>
	class tile_library
	{
	public:
		typedef TilePrototype tile_type;
		typedef decltype(tile_type{}.id) id_type;
		typedef decltype(tile_type{}.mass) mass_type;
		typedef decltype(tile_type{}.background) sprite_type;
		typedef std::bitset<mass_type::traverse_layers> bitset_type;

	public:
		tile_type const& at(uint32_t id) const
		{
			return m_dictionary.at(id);
		}
		tile_type & operator[](uint32_t id)
		{
			return m_dictionary[id];
		}
		template <typename Document>
		void load(Document && document)
		{
			for (auto const& node : document) {

				// read
				id_type id = node["id"];
				sprite_type name = node["background"];
				unsigned long long layers = node["traversable"];
				bool transparent = node["transparent"];

				// insert
				m_dictionary[id] = { id, name, { transparent, bitset_type(layers) } };
			}
		}

	private:
		std::map<id_type, tile_type> m_dictionary;
	};
}
