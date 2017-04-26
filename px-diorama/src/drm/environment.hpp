// name: environment.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// game logics

#pragma once

#include "perception.hpp"

#include "es/sprite_system.hpp"
#include "es/unit.hpp"
#include "es/unit_builder.hpp"
#include "es/unit_component.hpp"

#include "fn/generator.hpp"
#include "rl/map_chunk.hpp"

#include <px/fn/bsp.hpp>

#include <px/ui/panel.hpp>

#include "ui/inventory_list.hpp"
#include "ui/recipe_list.hpp"
#include "ui/target_panel.hpp"

#include <list>
#include <memory>

namespace px {

	class factory; // this thing is heavy

	class environment
	{
	public:

		// flow

		void update(perception & view) const;
		bool running() const noexcept;
		void shutdown() noexcept;
		void start();
		void clear();
		std::shared_ptr<unit> spawn(std::string const& name, point2 location);

		// user interface

		ui::panel & ui() noexcept;
		ui::panel const& ui() const noexcept;
		void layout_ui(rectangle bounds) noexcept;
		void expose_inventory(container_component * inventory);

		// serialization

		void save();
		void save(std::string const& name);
		void load();
		void load(std::string const& name);

		void lock_target(point2 relative_world_coordinates)
		{
			auto transform = m_player ? m_player->transform() : nullptr;

			m_hover = relative_world_coordinates + (transform ? transform->position() : point2{});

			if (m_target_panel)
			{
				m_target_panel->lock_target(find_any(m_hover));
				m_target_panel->lock_position(m_hover);
			}
		}
		void step(point2 const& direction)
		{
			auto transform = m_player ? m_player->transform() : nullptr;
			if (!transform) return;

			point2 destination = transform->position() + direction;

			if (!m_map.traversable(destination)) return;

			transform_component * blocking = find_any(destination);

			if (!blocking)
			{
				transform->place(destination);
			}
		}
		void use(unsigned int /*index*/)
		{
		}
		void activate(unsigned int /*mod*/)
		{
			auto target = find_any(m_hover);
			if (auto body = target ? target->linked<body_component>() : nullptr)
			{
				body->use(*body, *this);
			}
		}

		template <typename Archive>
		void save_unit(unit const& mobile, Archive & archive)
		{
			size_t total_components = mobile.components_size();
			archive(total_components);

			mobile.enumerate_components([&](auto & part) {
				if (auto transform = dynamic_cast<transform_component*>(part.get()))
				{
					archive(unit_component::transform);
					archive(*transform);
				}
				else if (auto sprite = dynamic_cast<sprite_component*>(part.get()))
				{
					archive(unit_component::sprite);

					size_t strlen = std::strlen(sprite->name);
					archive(strlen);
					archive.saveBinary(sprite->name, strlen);
				}
				else
				{
					archive(unit_component::undefined);
				}
			});
		}
		template <typename Archive>
		void load_unit(unit_builder & builder, Archive & archive)
		{
			size_t total_components;
			archive(total_components);

			for (size_t i = 0; i != total_components; ++i)
			{
				unit_component variant;
				archive(variant);

				switch (variant) {
				case unit_component::transform:
				{
					auto transform = builder.add_transform({});
					archive(*transform); // should be disabled by default, so we can write to internal values
				}
				break;
				case unit_component::sprite:
				{
					size_t strlen;
					archive(strlen);

					std::vector<char> name(strlen + 1, 0);
					archive.loadBinary(name.data(), strlen);

					builder.add_sprite(name.data());
				}
				break;
				case unit_component::container:
				case unit_component::storage:
				case unit_component::body:
				case unit_component::player:
					// there was unserilized component, just skip it
				case unit_component::undefined:
					break;
					// component defined, but not supported (version conflict?)
				default:
					throw std::runtime_error("px::environment::load_unit(builder, archive) - unknown component");
				}
			}
		}

		template <typename Archive>
		void save_units(Archive & archive)
		{
			size_t size = m_units.size();
			archive(size);
			for (auto const& unit : m_units)
			{
				save_unit(*unit, archive);
			}
		}
		template <typename Archive>
		void load_units(Archive & archive)
		{
			size_t size;
			archive(size);
			for (size_t i = 0; i != size; ++i)
			{
				unit_builder builder(m_factory.get());

				load_unit(builder, archive);
				auto mobile = builder.assemble();
				mobile->enable();
				m_units.push_back(mobile);

				m_player = mobile.get();
			}
		}

		template <typename Document>
		void add_sprite_atlas(Document && atlas, bool reverse_y)
		{
			m_sprites.add_atlas(std::forward<Document>(atlas), reverse_y);
		}

	public:
		~environment();
		environment();

	private:
		void setup_ui();
		void generate_terrain();

		transform_component * find_any(point2 position)
		{
			auto transform = m_player ? m_player->transform() : nullptr;
			auto world = transform ? transform->world() : nullptr;

			transform_component * result = nullptr;
			if (world) world->find(position.x(), position.y(), [&result](int /*x*/, int /*y*/, transform_component * obj) { result = obj; });

			return result;
		}

	private:
		bool m_run;

		point2 m_hover; // this variable uses relative world coordinates

		// components & units
		es::sprite_system m_sprites;
		std::list<std::shared_ptr<unit>> m_units;
		std::unique_ptr<factory> m_factory;
		unit * m_player;

		// terrain
		map_chunk<tile> m_map;

		// user interface
		ui::panel m_ui;
		ui::inventory_list * m_inventory;
		std::shared_ptr<ui::target_panel> m_target_panel;
	};
}