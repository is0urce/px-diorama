// name: sprite_system.hpp

#pragma once

#include <px/common/pool_chain.hpp>
#include "sprite_component.hpp"

#include <map>
#include <memory>

namespace px {
	class sprite_system final
	{
	public:
		typedef pool_chain<sprite_component, 100000> pool_type;

	public:
		template <typename Document>
		void add_texture(Document && metadoc, bool reverse_y)
		{
			for (auto const& frame : metadoc)
			{
				auto & img = m_meta[frame["name"]];
				img.sx = frame["sx"];
				img.sy = frame["sy"];
				img.dx = frame["dx"];
				img.dy = frame["dy"];
				img.layer = 0;
				img.texture = m_textures;
				img.glyph = '?';

				if (reverse_y)
				{
					img.sy = 1.0f - img.sy;
					img.dy = 1.0f - img.dy;
				}
			}
			++m_textures;
		}
		auto make_shared(std::string const& name)
		{
			image const& img = m_meta[name];
			auto result = m_pool->make_shared();

			static_cast<image&>(*result) = img;
			result->tint = color::white();

			return result;
		}
		void write(std::vector<std::vector<mesh_vertex>> & vertice_arrays, float x_offset, float y_offset)
		{
			if (vertice_arrays.size() < m_textures) throw std::runtime_error("px::sprite_system::write() - vertices array texture size not match with internal counter");

			m_pool->enumerate([&](auto const& sprite) {
				if (!sprite.active()) return; // continue
				auto* transform = sprite.linked<transform_component>();
				if (!transform) return; 

				int x = transform->x();
				int y = transform->y();

				float sx = x_offset + static_cast<float>(x);
				float sy = y_offset + static_cast<float>(y);
				float dx = x_offset + static_cast<float>(x + 1);
				float dy = y_offset + static_cast<float>(y + 1);

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
			, m_pool(std::make_unique<pool_type>())
		{
		}

	private:
		std::map<std::string, image> m_meta;
		std::unique_ptr<pool_type> m_pool; // this thing is heavy for a stack
		unsigned int m_textures;
	};
}