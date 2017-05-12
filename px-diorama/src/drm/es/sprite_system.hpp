// name: sprite_system.hpp

#pragma once

#include "sprite_component.hpp"
#include "transform_component.hpp"

#include <px/common/vector.hpp>
#include <px/es/basic_system.hpp>

#include <map>
#include <memory>
#include <vector>

#include "drm/vertex.hpp"

namespace px {
	namespace es {

		class sprite_system final
			: public basic_system<sprite_component, 100000>
		{
		public:
			typedef std::vector<std::vector<mesh_vertex>> arrays_type;

		public:
			auto make_shared(std::string const& name)
			{
				auto result = basic_system::make_shared();
				setup_component(*result, name);
				return result;
			}
			auto make_unique(std::string const& name)
			{
				auto result = basic_system::make_unique();
				setup_component(*result, name);
				return result;
			}
			auto make_std(std::string const& name)
			{
				auto result = basic_system::make_std();
				setup_component(*result, name);
				return result;
			}

			template <typename Document>
			void add_atlas(Document && atlas, bool reverse_y)
			{
				for (auto const& frame : atlas) {
					add_sprite<float>(frame["name"], frame["sx"], frame["sy"], frame["dx"], frame["dy"], reverse_y, static_cast<unsigned int>(m_batches.size()), 0, '?');
				}

				m_batches.emplace_back();
			}
			void update(double delta)
			{
				if (!m_camera) return;

				auto w = std::min(delta * 5, 1.0);

				vector2 camera_position = interpolate(*m_camera, w);

				// clear previous data
				for (auto & batch : m_batches) {
					batch.resize(0);
				}

				// compose arrays
				enumerate([&](auto const& sprite) {
					if (!sprite.active()) return; // continue

					transform_component * transform = sprite.linked<transform_component>();

					if (!transform) return;

					vector2 pos = interpolate(*transform, w) - camera_position;
					auto x = pos.x();
					auto y = pos.y();

					float sx = static_cast<float>(x);
					float sy = static_cast<float>(y);
					float dx = static_cast<float>(x + 1);
					float dy = static_cast<float>(y + 1);

					auto & vertices = m_batches[sprite.texture];

					vertices.push_back({ { sx, dy }, { sprite.sx, sprite.dy } });
					vertices.push_back({ { sx, sy }, { sprite.sx, sprite.sy } });
					vertices.push_back({ { dx, sy }, { sprite.dx, sprite.sy } });
					vertices.push_back({ { dx, dy }, { sprite.dx, sprite.dy } });
				});
			}
			void assign_camera(transform_component const* camera) noexcept
			{
				m_camera = camera;
			}
			arrays_type const& batches() const noexcept
			{
				return m_batches;
			}

		public:
			sprite_system()
				: m_camera(nullptr)
			{
			}

		private:
			static vector2 interpolate(transform_component const& pawn, double w)
			{
				return vector2(pawn.last_position()).lerp(pawn.position(), w);
			}
			void setup_component(sprite_component & element, std::string const& name)
			{
				image const& img = m_meta[name];

				static_cast<image&>(element) = img;
				element.tint = color::white();
			}
			template <typename Real>
			void add_sprite(std::string const& name, Real sx, Real sy, Real dx, Real dy, bool reverse_y, unsigned int texture, unsigned int layer, unsigned int glyph)
			{
				auto ib_pair = m_meta.emplace(name, image{});
				auto & img = ib_pair.first->second; // get iterator, get value

				// set image attributes
				img.sx = sx;
				img.sy = reverse_y ? 1 - sy : sy;
				img.dx = dx;
				img.dy = reverse_y ? 1 - dy : dy;
				img.layer = layer;
				img.texture = texture;
				img.glyph = glyph;
				img.name = ib_pair.first->first.c_str(); // get iterator, get key, get c_str pointer
			}

		private:
			std::map<std::string, image>	m_meta;		// name -> texture dirctionary
			arrays_type						m_batches;	// vertex data for rendering
			transform_component const*		m_camera;	// central camera position
		};
	}
}