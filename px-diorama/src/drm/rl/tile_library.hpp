// name: tile_library.hpp
// type: c++
// auth: is0urce
// type: template class

#pragma once

#include <cstdint>
#include <bitset>
#include <map>

namespace px {

	// where TilePrototype is tile_prototype
	template <typename TilePrototype>
	class tile_library
	{
	public:
		typedef TilePrototype blueprint_type;
		typedef decltype(blueprint_type{}.id) id_type;
		typedef decltype(blueprint_type{}.mass) mass_type;
		typedef decltype(blueprint_type{}.background) sprite_type;
		typedef std::bitset<mass_type::traverse_layers> bitset_type;

	public:
		blueprint_type const& at(uint32_t id) const
		{
			return m_dictionary.at(id);
		}
		blueprint_type & operator[](uint32_t id)
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

				m_dictionary[id] = { id, name, { transparent, bitset_type(layers) } }; // and insert
			}
		}

	private:
		std::map<id_type, blueprint_type> m_dictionary;
	};
}
