// name: sprite_system.hpp

#pragma once

#include "sprite_component.hpp"

#include <px/common/vector.hpp>
#include <px/es/basic_system.hpp>

#include <map>
#include <vector>
#include <string>

#include "../vertex.hpp"

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

			void update(double delta_time);
			void assign_camera(transform_component const* camera) noexcept;
			arrays_type const& batches() const noexcept;
			void set_cropping(float far) noexcept;

		public:
			sprite_system();

		private:
			void setup_component(sprite_component & element, std::string const& name);
			void load(std::string const& textures, bool reverse_y);
			void add_sprite(std::string const& name, unsigned int texture, float sx, float sy, float dx, float dy);

		private:
			std::map<std::string, image>	m_meta;		// name -> texture dirctionary
			arrays_type						m_batches;	// vertex data for rendering
			transform_component const*		m_camera;	// central camera position
			float							m_far;		// cropping
		};
	}
}