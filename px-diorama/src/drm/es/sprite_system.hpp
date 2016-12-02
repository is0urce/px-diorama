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
		void update(std::vector<std::vector<vertex>> & vertice_arrays, std::vector<size_t> & sizes)
		{
			if (vertice_arrays.size() != m_textures) throw std::runtime_error("px::sprite_system::update() - vertices array texture size not match with internal counter");

			for (auto & vertice_array : vertice_arrays)
			{
				vertice_array.resize(m_pool.size() * 4);
			}

			sizes.assign(m_textures, 0);
			m_pool.enumerate([&](auto const& sprite) {
				auto* transform = sprite.linked<transform_component>();
				if (!transform) return; // continue

				auto & vertices = vertice_arrays[sprite.texture];
				size_t i = sizes[sprite.texture];

				vertices[i * 4 + 0].pos.x = static_cast<float>(transform->x() - 1);
				vertices[i * 4 + 0].pos.y = static_cast<float>(transform->y() + 1);
				vertices[i * 4 + 0].texture.x = sprite.sx;
				vertices[i * 4 + 0].texture.y = sprite.dy;

				vertices[i * 4 + 1].pos.x = static_cast<float>(transform->x() - 1);
				vertices[i * 4 + 1].pos.y = static_cast<float>(transform->y() - 1);
				vertices[i * 4 + 1].texture.x = sprite.sx;
				vertices[i * 4 + 1].texture.y = sprite.sy;

				vertices[i * 4 + 2].pos.x = static_cast<float>(transform->x() + 1);
				vertices[i * 4 + 2].pos.y = static_cast<float>(transform->y() - 1);
				vertices[i * 4 + 2].texture.x = sprite.dx;
				vertices[i * 4 + 2].texture.y = sprite.sy;

				vertices[i * 4 + 3].pos.x = static_cast<float>(transform->x() + 1);
				vertices[i * 4 + 3].pos.y = static_cast<float>(transform->y() + 1);
				vertices[i * 4 + 3].texture.x = sprite.dx;
				vertices[i * 4 + 3].texture.y = sprite.dy;

				sizes[sprite.texture] += 4;
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