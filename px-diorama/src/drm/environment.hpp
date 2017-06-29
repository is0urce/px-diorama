// name: environment.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// game logics

#pragma once

#include "rl/notification.hpp"
#include "rl/tile_terrain.hpp"
#include "repository.hpp"

#include "ui/menu.hpp"
#include "vfx.hpp"

#include <px/common/coordinate.hpp>
#include <px/common/coordinate_ext.hpp> // lex_less

#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace px {

	class factory;
	class perception;

	class container_component;
	class body_component;
	class transform_component;
	class unit;

	class environment
	{
	public:
		typedef std::shared_ptr<unit> unit_ptr;
		typedef tile_terrain<tile_instance> terrain_type;

	public:

		// flow

		void update(perception & view, double time);
		bool running() const noexcept;
		void shutdown() noexcept;
		void start();
		void end();

		// actions

		void target(point2 relative_coordinates);
		void step(point2 const& direction);
		void use(unsigned int ability_index);
		void activate(unsigned int modifier);

		// user interface

		ui::panel * ui() noexcept;
		ui::panel const* ui() const noexcept;

		// interaction

		void expose_inventory(container_component * inventory);
		void open_workshop(unsigned int workshop);
		void popup(point2 location, std::string text, color tint, float size);
		void visual(std::string const& tag, point2 from, point2 destination, transform_component const* follow);
		std::tuple<int, int, bool, bool> hit(body_component const&, body_component const&) const;

		// serialization

		void save();
		void load();
		void save_game(std::string const& save_name);
		void load_game(std::string const& save_name);
		void archive_scene(point2 const& cell);
		void restore_scene(point2 const& cell);

		// units

		void spawn(unit_ptr unit);
		unit_ptr create_dummy(std::string const& unit_name, point2 location);
		void export_unit(unit const& mobile, std::string const& blueprint_name) const;
		unit_ptr import_unit(std::string const& blueprint_name);
		unit_ptr import_unit(std::string const& blueprint_name, point2 location);
		size_t mass_export(point2 const& location);

		// terrain

		terrain_type & terrain();

	public:
		~environment();
		environment();
		environment(environment const&) = delete;
		environment & operator=(environment const&) = delete;

	private:
		template <typename Action>
		void turn(Action && intent_action, int time);
		void turn_begin();
		void turn_end();
		void impersonate(transform_component * player);
		transform_component * find_any(point2 const& position);

	private:
		std::unique_ptr<factory>	m_factory;			// for assembling units, release last

		bool						m_run;				// if engine is working
		unsigned int				m_turn;				// current turn
		unsigned int				m_last_turn;		// last updated turn
		double						m_last_time;		// last time of update

		repository					m_save;				// save directory
		terrain_type				m_terrain;			// terrain
		std::vector<unit_ptr>		m_units;			// scene

		transform_component *		m_player;			// player transform
		point2						m_hover;			// current hovered tile
		ui::menu					m_ui;				// user interface
		std::vector<vfx>			m_visuals;			// visual effects container
		std::map<point2, std::vector<notification>, lex_less> m_notifications;	// popups container
	};
}