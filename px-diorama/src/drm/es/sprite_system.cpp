// name: sprite_system.cpp
// type: c++ code
// auth: is0urce
// desc: class methods implementation

#include "sprite_system.hpp"

#include "transform_component.hpp"
#include "drm/settings.hpp"

#include <px/common/assert.hpp>

#include <fstream>

#include <json.hpp>

namespace px {
	namespace es {

		sprite_system::sprite_system()
			: m_camera(nullptr)
		{
			load(textureatlas_path, true);
		}
		void sprite_system::update(double delta_time)
		{
			vector2 camera_position = m_camera ? m_camera->interpolate(delta_time) : vector2{ 0, 0 };

			// clear previous data
			for (auto & sprite_batch : m_batches) {
				sprite_batch.resize(0);
			}

			// compose arrays
			enumerate([&](auto const& sprite) {
				if (!sprite.active()) return; // continue

				transform_component * transform = sprite.linked<transform_component>();

				px_assert(transform);

				if (!transform) return;

				vector2 pos = transform->interpolate(delta_time) - camera_position;
				float sx = static_cast<float>(pos.x()) - sprite.tx; // center offset
				float sy = static_cast<float>(pos.y()) - sprite.ty;
				float dx = sx + sprite.mx; // precalculated magnitudes
				float dy = sy + sprite.my;

				auto & vertices = m_batches[sprite.texture];

				vertices.push_back({ { sx, dy },{ sprite.sx, sprite.dy } });
				vertices.push_back({ { sx, sy },{ sprite.sx, sprite.sy } });
				vertices.push_back({ { dx, sy },{ sprite.dx, sprite.sy } });
				vertices.push_back({ { dx, dy },{ sprite.dx, sprite.dy } });
			});
		}
		void sprite_system::assign_camera(transform_component const* camera) noexcept
		{
			m_camera = camera;
		}
		sprite_system::arrays_type const& sprite_system::batches() const noexcept
		{
			return m_batches;
		}
		void sprite_system::setup_component(sprite_component & element, std::string const& name)
		{
			image const& img = m_meta[name];

			static_cast<image&>(element) = img;
			element.tint = color::white();
		}
		void sprite_system::load(std::string const& textures, bool reverse_y)
		{
			std::ifstream textures_stream(textures);
			if (!textures_stream.is_open()) throw std::runtime_error("error opening file path=" + std::string(textures));
			auto texture_configuration = nlohmann::json::parse(textures_stream);

			for (auto const& texture : texture_configuration["textures"]) {

				std::string atlas_path = texture["atlas"];

				std::ifstream atlas_stream(atlas_path);
				if (!textures_stream.is_open()) throw std::runtime_error("error opening file path=" + std::string(atlas_path));
				auto atlas = nlohmann::json::parse(atlas_stream);

				for (auto const& frame : atlas["atlas"]) {
					float sx = frame["sx"];
					float sy = frame["sy"];
					float dx = frame["dx"];
					float dy = frame["dy"];
					sy = reverse_y ? 1 - sy : sy;
					dy = reverse_y ? 1 - dy : dy;
					add_sprite(frame["name"], static_cast<unsigned int>(m_batches.size()), sx, sy, dx, dy);
				}
				m_batches.emplace_back();
			}
		}
		void sprite_system::add_sprite(std::string const& name, unsigned int texture, float sx, float sy, float dx, float dy)
		{
			auto ib_pair = m_meta.emplace(name, image{ sx, sy, dx, dy, 0.0, 0.0, 1.0, 1.0, texture, 0, '?', nullptr });

			ib_pair.first->second.name = ib_pair.first->first.c_str(); // get iterator, get image, get name pointer = get iterator, get string key, get c_str pointer
		}
	}
}