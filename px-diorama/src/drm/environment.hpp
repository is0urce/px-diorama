// name: environment.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// game logics

#pragma once

#include "rl/notification.hpp"
#include "repository.hpp"

#include "ui/menu.hpp"

#include <px/common/coordinate.hpp>
#include <px/common/coordinate_ext.hpp> // lex_less
#include <px/rl/traverse_options.hpp>
#include <px/rl/traverse.hpp>

#include <map>
#include <memory>
#include <random>
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

	template <typename Tile>
	class tile_terrain;
	struct tile_instance;
	struct vfx;

	class environment
	{
	public:
		typedef unit unit_type;
		typedef std::shared_ptr<unit_type> unit_ptr;
		typedef tile_terrain<tile_instance> terrain_type;
		typedef rl::traverse_options<rl::traverse> traverse_type;

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

		unsigned int distance(point2 const& a, point2 const& b) const noexcept;
		bool traversable(point2 const& location, traverse_type traverse) const;
		bool transparent(point2 const& location) const;
		void expose_inventory(container_component * inventory);
		void loot(body_component * user, container_component * inventory);
		void open_workshop(unsigned int workshop);
		void popup(point2 location, std::string text, color tint, float size);
		void visual(std::string const& tag, point2 from, point2 destination, transform_component const* follow);
		std::tuple<int, int, bool, bool> hit(body_component const&, body_component const&);
		void damage(body_component & body, int damage); // damage with popup

		// serialization

		bool editor() const noexcept;
		void save();
		void load();
		void save(std::string const& save_name);
		void load(std::string const& save_name);

		// units

		transform_component const* player() const noexcept;
		void enable(unit_ptr & unit);
		void spawn(unit_ptr unit);
		void edit(unit_ptr unit);
		unit * edited();
		unit_ptr create_empty(std::string const& tag, point2 location);
		unit_ptr import_unit(std::string const& blueprint_name, point2 location);
		unit_ptr compile_unit(std::string const& scheme_name, point2 location);
		void export_unit(unit const& mobile, std::string const& blueprint_name) const;
		size_t mass_export(point2 const& location);
		unit_ptr create_dummy(std::string const& tag, point2 location);

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
		void turn_pass(unsigned int turns);
		void impersonate(transform_component * player);
		transform_component * find_any(point2 const& position);

		void save_main(); // save main scene
		void load_main(); // load main scene
		void save_meta(); // save misc states
		void load_meta(); // load misc states
		void archive_scene(point2 const& cell); // move scene to repository
		void restore_scene(point2 const& cell); // restore scene from repository

	private:
		std::unique_ptr<factory>		m_factory;			// for assembling units, release last
		std::unique_ptr<terrain_type>	m_terrain;			// terrain

		bool							m_run;				// if engine is working
		unsigned int					m_turn;				// current turn
		unsigned int					m_last_turn;		// last updated turn
		double							m_last_time;		// last time of update

		bool							m_editor;			// editor mode
		repository						m_base;				// repository of base game setup
		repository						m_repository;		// save directory of current game

		std::vector<unit_ptr>			m_units;			// units on scene
		unit_ptr						m_edited;			// current edited unit

		transform_component *			m_player;			// player transform
		std::mt19937					m_rng;
		point2							m_hover;			// current hovered tile
		ui::menu						m_ui;				// user interface

		std::unique_ptr<std::vector<vfx>>						m_vfx;				// visual effects container
		std::map<point2, std::vector<notification>, lex_less>	m_notifications;	// popups container
	};
}