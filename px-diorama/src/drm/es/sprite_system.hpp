// name: sprite_system.hpp

#pragma once

#include <px/common/pool_chain.hpp>
#include "sprite_component.hpp"

#include <map>
#include <memory>

#include <px/es/basic_system.hpp>

namespace px {
	namespace es {

		class sprite_system final
			: public basic_system<sprite_component, 100000>
		{
		public:
			auto make_shared(std::string const& name)
			{
				auto result = basic_system::make_shared();
				setup(*result, name);
				return result;
			}
			auto make_unique(std::string const& name)
			{
				auto result = basic_system::make_unique();
				setup(*result, name);
				return result;
			}
			auto make_std(std::string const& name)
			{
				auto result = basic_system::make_std();
				setup(*result, name);
				return result;
			}

			template <typename Document>
			void add_texture(Document && document, bool reverse_y)
			{
				for (auto const& frame : document)
				{
					add_sprite<float>(frame["name"], frame["sx"], frame["sy"], frame["dx"], frame["dy"], reverse_y, m_textures, 0, '?');
				}
				++m_textures;
			}
			void write(std::vector<std::vector<mesh_vertex>> & vertice_arrays, float x_offset, float y_offset)
			{
				if (vertice_arrays.size() < m_textures) throw std::runtime_error("px::sprite_system::write() - vertices array texture size not match with internal counter");

				enumerate([&](auto const& sprite) {
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
			{
			}

		private:
			void setup(sprite_component & element, std::string const& name)
			{
				image const& img = m_meta[name];

				static_cast<image&>(element) = img;
				element.tint = color::white();
			}
			template <typename Real>
			void add_sprite(std::string const& name, Real sx, Real sy, Real dx, Real dy, bool reverse_y, unsigned int texture, unsigned int layer, unsigned int glyph)
			{
				auto & img = m_meta[name];
				img.sx = sx;
				img.sy = reverse_y ? 1 - sy : sy;
				img.dx = dx;
				img.dy = reverse_y ? 1 - dy : dy;
				img.layer = layer;
				img.texture = texture;
				img.glyph = glyph;
			}

		private:
			std::map<std::string, image> m_meta;
			unsigned int m_textures;
		};
	}
}