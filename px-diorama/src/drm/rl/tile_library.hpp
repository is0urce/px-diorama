// name: tile_library.hpp

#pragma once

#include <bitset>
#include <cstdint>
#include <map>
#include <string>

namespace px {

	template <typename Attributes>
	struct tile_attributes
	{
		uint32_t id;
		std::string background;
		Attributes mass;
	};

	// where Props is tile_props<Mass>
	template <typename Tile>
	class tile_library
	{
	public:
		typedef Tile tile_type;
		typedef decltype(tile_type{}.mass) mass_type;
		typedef std::bitset<mass_type::traverse_layers> bitset_type;

	public:
		tile_type const& operator[](uint32_t id) const
		{
			return m_dictionary.at(id);
		}
		template <typename Document>
		void load(Document && document)
		{
			for (auto const& node : document) {

				// read
				uint32_t id = node["id"];
				std::string name = node["background"];
				unsigned long long layers = node["traversable"];
				bool transparent = node["transparent"];

				// insert
				m_dictionary[id] = { id, name, { transparent, bitset_type(layers) } };
			}
		}

	private:
		std::map<uint32_t, Tile> m_dictionary;
	};
}
