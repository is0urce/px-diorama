#pragma once

#include <px/common/pool_chain.hpp>
#include "sprite_component.hpp"

#include <map>

namespace px {
	class sprite_system final
	{
	public:
		typedef pool_chain<sprite_component, 200> pool_type;
	public:
		template <typename Document>
		void add_texture(Document const& doc, float reverse_y)
		{
			for (auto const& meta : doc)
			{
				auto & img = m_meta[meta["name"]];
				float sx = meta["sx"];
				float sy = meta["sy"];
				float dx = meta["dx"];
				float dy = meta["dy"];
				if (reverse_y)
				{
					sy = 1.0f - sy;
					dy = 1.0f - dy;
				}
				img = { sx, sy, dx, dy };
				img.layer = 0;
				img.texture = m_textures;
				img.glyph = '?';
			}
			++m_textures;
		}
		auto make_shared(std::string const& name)
		{
			image const& img = m_meta[name];
			auto result = m_pool.make_shared();

			static_cast<image&>(*result) = img;
			result->tint = color::white();

			return result;
		}
		void write(std::vector<std::vector<vertex>> & vertice_arrays)
		{
			if (vertice_arrays.size() < m_textures) throw std::runtime_error("px::sprite_system::write() - vertices array texture size not match with internal counter");

			m_pool.enumerate([&](auto const& sprite) {
				if (!sprite.active()) return; // continue
				auto* transform = sprite.linked<transform_component>();
				if (!transform) return; 

				float sx = static_cast<float>(transform->x());
				float sy = static_cast<float>(transform->y());
				float dx = sx + 1;
				float dy = sy + 1;

				auto & vertices = vertice_arrays[sprite.texture];

				vertices.push_back({ { sx, dy }, { sprite.sx, sprite.dy } });
				vertices.push_back({ { sx, sy }, { sprite.sx, sprite.sy } });
				vertices.push_back({ { dx, sy }, { sprite.dx, sprite.sy } });
				vertices.push_back({ { dx, dy }, { sprite.dx, sprite.dy } });
			});
		}

	public:
		sprite_system()
			: m_textures(0)
		{
		}

	private:
		std::map<std::string, image> m_meta;
		pool_type m_pool;
		unsigned int m_textures;
	};
}