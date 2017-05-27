// name: environment.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// game logics

#pragma once

#include "es/unit.hpp"
#include "rl/notification.hpp"
#include "rl/tile_chunk.hpp"
#include "ui/menu.hpp"
#include "vfx.hpp"

#include <px/common/coordinate.hpp>
#include <px/common/coordinate_ext.hpp> // lex_less

#include <list>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace px {

	class factory;
	class perception;
	class body_component;

	class environment
	{
	public:

		void add_spritesheet(std::string const& path, bool reverse_y);

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
		void activate(unsigned int mod);

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
		void save(std::string const& name);
		void load();
		void load(std::string const& name);

	public:
		~environment();
		environment();
		environment(environment const&) = delete;
		environment & operator=(environment const&) = delete;

	private:
		void impersonate(transform_component * player);
		std::shared_ptr<unit> spawn(std::string const& name, point2 location);
		void turn_begin();
		void turn_end();
		transform_component * find_any(point2 const& position);

		template <typename Action>
		void turn(Action && intent_action, int time);

		template <typename Archive>
		void save_units(Archive & archive) const;
		template <typename Archive>
		void load_units(Archive & archive);
		template <typename Archive>
		void save_unit(unit const& mobile, Archive & archive) const;
		template <typename Archive, typename Builder>
		void load_unit(Builder & builder, Archive & archive);

	private:
		std::unique_ptr<factory>			m_factory;			// for assembling units, release last

		bool								m_run;				// if engine is working
		unsigned int						m_turn;				// current turn
		unsigned int						m_last_turn;		// last updated turn
		double								m_last_time;		// last time of update

		tile_chunk<tile_instance>			m_map;				// terrain
		std::vector<std::shared_ptr<unit>>	m_units;			// scene
		transform_component *				m_player;			// player transform

		point2								m_hover;			// current hovered tile
		ui::menu							m_ui;				// user interface
		std::list<vfx>						m_visuals;			// visual effects container
		std::map<point2, std::vector<notification>, lex_less> m_notifications;	// popups container
	};
}