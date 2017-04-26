// name: environment.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// game logics

#pragma once

#include "es/unit.hpp"

#include "fn/generator.hpp"
#include "rl/terrain.hpp"

#include <px/fn/bsp.hpp>

#include <px/ui/panel.hpp>

#include "ui/inventory_list.hpp"
#include "ui/recipe_list.hpp"
#include "ui/target_panel.hpp"

#include <list>
#include <memory>
#include <string> // save names

namespace px {

	class factory;
	class perception;

	class environment
	{
	public:

		// flow

		void update(perception & view) const;
		bool running() const noexcept;
		void shutdown() noexcept;
		void start();
		void clear();
		void impersonate(transform_component * player);
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

		void target(point2 relative_world_coordinates)
		{
			m_hover = relative_world_coordinates + (m_player ? m_player->position() : point2(0, 0));

			if (m_target_panel) m_target_panel->lock(m_hover, find_any(m_hover));
		}
		void step(point2 const& direction)
		{
			if (!m_player) return;

			point2 destination = m_player->position() + direction;

			if (!m_map.traversable(destination)) return;

			auto blocking = find_any(destination);

			if (!blocking) {
				m_player->place(destination);
			}
		}
		void use(unsigned int /*index*/)
		{
		}
		void activate(unsigned int /*mod*/)
		{
			auto target = find_any(m_hover);
			if (auto body = target ? target->linked<body_component>() : nullptr) {
				body->use(*body, *this);
			}
		}

		void add_spritesheet(std::string const& path, bool reverse_y);


	public:
		~environment();
		environment();

	private:
		void setup_ui();
		void generate_terrain();

		template <typename Archive>
		void save_units(Archive & archive);
		template <typename Archive>
		void load_units(Archive & archive);
		template <typename Archive>
		void save_unit(unit const& mobile, Archive & archive);
		template <typename Archive, typename Builder>
		void load_unit(Builder & builder, Archive & archive);

		transform_component * find_any(point2 position)
		{
			transform_component * result = nullptr;

			auto world = m_player ? m_player->world() : nullptr;
			if (world) world->find(position.x(), position.y(), [&result](int /*x*/, int /*y*/, transform_component * obj) { result = obj; });

			return result;
		}

	private:
		bool m_run;

		point2 m_hover; // this variable uses relative world coordinates

		// components & units
		std::unique_ptr<factory> m_factory;
		std::list<std::shared_ptr<unit>> m_units;
		transform_component * m_player;

		// terrain
		terrain_chunk<tile> m_map;

		// user interface
		ui::panel m_ui;
		ui::inventory_list * m_inventory;
		std::shared_ptr<ui::target_panel> m_target_panel;
	};
}