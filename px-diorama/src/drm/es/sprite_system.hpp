#pragma once

#include <px/common/pool_chain.hpp>
#include "sprite_component.hpp"

#include <map>

namespace px {
	class sprite_system final
	{
	public:
		typedef pool_chain<sprite_component, 100> pool_type;
	public:
		template <typename Document>
		void add_texture(Document const& doc, unsigned int n)
		{
			for (auto const& meta : doc)
			{
				auto & img = m_meta[meta["name"]];
				img = { meta["sx"], meta["sy"], meta["dx"], meta["dy"] };
				img.layer = 0;
				img.texture = n;
				img.glyph = '?';
			}
		}
		auto make_shared(std::string const& name)
		{
			image const& img = m_meta[name];
			auto result = m_pool.make_shared();

			static_cast<image&>(*result) = img;

			return result;
		}

	private:
		std::map<std::string, image> m_meta;
		pool_type m_pool;
	};
}