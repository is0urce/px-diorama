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
				float sx = meta["sx"];
				float sy = meta["sy"];
				float dx = meta["dx"];
				float dy = meta["dy"];
				img = { sx, 1.0f - sy, dx, 1.0f - dy };
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
		void update(std::vector<vertex> & vertices)
		{
			vertices.resize(m_pool.size() * 4);
			size_t i = 0;
			m_pool.enumerate([&](sprite_component const& sprite) {
				auto* transform = sprite.linked<transform_component>();

				vertices[i * 4 + 0].pos.x = static_cast<float>(transform->current.x() - 1);
				vertices[i * 4 + 0].pos.y = static_cast<float>(transform->current.y() + 1);
				vertices[i * 4 + 0].texture.x = sprite.sx;
				vertices[i * 4 + 0].texture.y = sprite.dy;

				vertices[i * 4 + 1].pos.x = static_cast<float>(transform->current.x() - 1);
				vertices[i * 4 + 1].pos.y = static_cast<float>(transform->current.y() - 1);
				vertices[i * 4 + 1].texture.x = sprite.sx;
				vertices[i * 4 + 1].texture.y = sprite.sy;

				vertices[i * 4 + 2].pos.x = static_cast<float>(transform->current.x() + 1);
				vertices[i * 4 + 2].pos.y = static_cast<float>(transform->current.y() - 1);
				vertices[i * 4 + 2].texture.x = sprite.dx;
				vertices[i * 4 + 2].texture.y = sprite.sy;
			
				vertices[i * 4 + 3].pos.x = static_cast<float>(transform->current.x() + 1);
				vertices[i * 4 + 3].pos.y = static_cast<float>(transform->current.y() + 1);
				vertices[i * 4 + 3].texture.x = sprite.dx;
				vertices[i * 4 + 3].texture.y = sprite.dy;

				i += 4;;
			});
		}

	private:
		std::map<std::string, image> m_meta;
		pool_type m_pool;
	};
}