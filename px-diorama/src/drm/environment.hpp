// name: environment.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// game logics

#pragma once

#include "perception.hpp"

#include "es/sprite_system.hpp"
#include "es/unit.hpp"
#include "es/factory.hpp"
#include "es/unit_builder.hpp"
#include "es/unit_data.hpp"

#include "fn/generator.hpp"
#include "rl/map_chunk.hpp"

#include <px/fn/bsp.hpp>

#include <px/ui/panel.hpp>

#include "ui/inventory_list.hpp"
#include "ui/recipe_list.hpp"
#include "ui/target_panel.hpp"

#include <list>

namespace px {

	class environment
	{
	public:
		bool running() const noexcept;
		void shutdown() noexcept;
		ui::panel & ui() noexcept;
		ui::panel const& ui() const noexcept;
		void layout_ui(rectangle bounds) noexcept;
		void expose_inventory(container_component * /*inventory*/);

		void save();
		void save(std::string const& name);
		void load();
		void load(std::string const& name);


		void lock_target(point2 relative_world_coordinates)
		{
			transform_component * transform = m_player ? m_player->transform() : nullptr;
			if (!transform) return;

			m_hover = relative_world_coordinates + transform->position();

			if (m_target_panel)
			{
				m_target_panel->lock_target(find_any(m_hover));
				m_target_panel->lock_position(m_hover);
			}
		}
		void step(point2 const& direction)
		{
			transform_component * transform = m_player ? m_player->transform() : nullptr;
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
			auto body = target ? target->linked<body_component>() : nullptr;

			if (body)
			{
				body->use(*body, *this);
			}
		}

		void start()
		{
			// terrain

			m_map.resize({ 100, 100 });
			m_map.enumerate([this](auto const& point, auto & tile) {
				tile.transform.move(point);
				tile.sprite = m_sprites.make_unique("#");

				tile.sprite->connect(&tile.transform);
				tile.sprite->activate();

				tile.make_wall();
			});

			std::mt19937 rng;
			fn::dig_generator dig(100, 100);
			dig.generate(rng, 5, 12, 1, 15);
			matrix2<unsigned char> map(100, 100);
			dig.rasterize(map);
			map.enumerate([&](auto const& point, unsigned char t) {
				if (t == 0) return;
				auto & tile = m_map[point];

				tile.sprite = m_sprites.make_unique(".");

				tile.sprite->connect(&tile.transform);
				tile.sprite->activate();

				tile.make_ground();
			});

			// units

			m_player = spawn("@", { 54, 46 }).get();

			spawn("m", { 55, 46 });
			spawn("x", { 53, 47 });
			spawn("m_snake", { 50, 50 });

			spawn("p_table", { 55, 47 });
			spawn("p_bookshelf", { 54, 47 });
			spawn("p_locker", { 55, 48 });
			spawn("p_box", { 55, 49 });

			// ui
			if (m_inventory) m_inventory->set_container(m_player->transform()->linked<body_component>()->linked<container_component>());
		}
		void write(perception & view) const
		{
			// render sprites
			if (transform_component * transform = m_player ? m_player->transform() : nullptr)
			{
				float x_offset = -static_cast<float>(transform->x());
				float y_offset = -static_cast<float>(transform->y());
				m_sprites.write(view.batches(), x_offset, y_offset);
			}

			// render user interface
			m_ui.draw(view.canvas());
		}
		std::shared_ptr<unit> spawn(std::string const& name, point2 location)
		{
			unit_builder builder(&m_factory);
			auto transform = builder.add_transform(location);
			auto body = builder.add_body();
			auto sprite = builder.add_sprite(name);
			auto container = builder.add_container();
			auto storage = builder.add_storage();

			// setup
			for (int i = 0; i != 100; ++i)
			{
				auto itm = std::make_shared<rl::item>();
				itm->set_name("item #" + std::to_string(i));
				itm->add({ rl::effect::ore_power, 0x100, 0x500 });
				container->add(itm);
			}
			body->health().create();
			body->set_name(name);

			// track
			auto result = builder.assemble();

			result->enable();
			m_units.push_back(result);

			return result;
		}

		template <typename Archive>
		void save_unit(unit const& mobile, Archive & archive)
		{
			unit_data data;
			auto transform = mobile.component<transform_component>();
			auto body = mobile.component<body_component>();

			data.transform = transform;
			data.body = body;
			archive(data);

			//if (data.transform)
			//{
			//	//transform->serialize(archive);
			//}
		}
		template <typename Archive>
		void load_unit(unit_builder & /*builder*/, Archive & archive)
		{
			unit_data data;
			archive(data);

			//if (data.transform)
			//{
			//	//auto transform = builder.add_transform();
			//	//transform->serialize(archive);
			//}
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
				unit_builder builder(&m_factory);
				load_unit(builder, archive);
				m_units.push_back(builder.assemble());
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
		transform_component * find_any(point2 position)
		{
			transform_component * result = nullptr;

			auto transform = m_player ? m_player->transform() : nullptr;
			auto world = transform ? transform->world() : nullptr;

			if (world) world->find(position.x(), position.y(), [&](int /*x*/, int /*y*/, transform_component * obj) { result = obj;	});

			return result;
		}

	private:
		bool m_run;

		point2 m_hover; // this variable uses relative world coordinates

		// components & units
		es::sprite_system m_sprites;
		factory m_factory;
		std::list<std::shared_ptr<unit>> m_units;
		unit * m_player;

		// terrain
		map_chunk<tile> m_map;

		// user interface
		ui::panel m_ui;
		ui::inventory_list * m_inventory;
		std::shared_ptr<ui::target_panel> m_target_panel;
	};
}