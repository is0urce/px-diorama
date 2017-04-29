// name: environment.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// game logics

#pragma once

#include "es/unit.hpp"

#include "rl/terrain.hpp"
#include "ui/menu.hpp"

#include "fn/generator.hpp"
#include <px/fn/bsp.hpp>

#include <px/ui/panel.hpp>

#include <list>
#include <memory>
#include <string> // save names

namespace px {

	class factory;
	class perception;

	class environment
	{
	public:

		void add_spritesheet(std::string const& path, bool reverse_y);

		// flow

		void update(perception & view) const;
		bool running() const noexcept;
		void shutdown() noexcept;
		void start();
		void clear();
		void impersonate(transform_component * player);
		std::shared_ptr<unit> spawn(std::string const& name, point2 location);

		// actions

		void target(point2 relative_world_coordinates);
		void step(point2 const& direction);
		void use(unsigned int ability_index);
		void activate(unsigned int mod);

		// user interface

		ui::panel * ui() noexcept;
		ui::panel const* ui() const noexcept;
		void expose_inventory(container_component * inventory);
		void open_workshop(unsigned int workshop);

		// serialization

		void save();
		void save(std::string const& name);
		void load();
		void load(std::string const& name);

	public:
		~environment();
		environment();

	private:
		void generate_terrain();
		transform_component * find_any(point2 const& position);

		template <typename Archive>
		void save_units(Archive & archive);
		template <typename Archive>
		void load_units(Archive & archive);
		template <typename Archive>
		void save_unit(unit const& mobile, Archive & archive);
		template <typename Archive, typename Builder>
		void load_unit(Builder & builder, Archive & archive);

	private:
		bool								m_run;		// if engine is working
		std::unique_ptr<factory>			m_factory;	// for assembling units, release last

		point2								m_hover;	// current hovered tile (uses relative world coordinates)
		ui::menu							m_ui;		// user interface

		terrain_chunk<tile>					m_map;		// terrain
		std::list<std::shared_ptr<unit>>	m_units;	// scene
		transform_component *				m_player;	// player transform
	};
}