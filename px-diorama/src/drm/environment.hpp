// name: environment.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// game logics

#pragma once

#include "perception.hpp"

#include "es/transform_system.hpp"
#include "es/sprite_system.hpp"
#include "es/body_system.hpp"
#include "es/container_system.hpp"
#include "es/unit.hpp"

#include "rl/map_chunk.hpp"
#include "fn/generator.hpp"

#include <px/common/colors.hpp>
#include <px/fn/bsp.hpp>
#include <px/ui/canvas.hpp>
#include <px/ui/panel.hpp>
#include <px/ui/board.hpp>
#include <px/ui/text.hpp>
#include <px/ui/button.hpp>
#include <px/ui/toggle.hpp>

#include "ui/inventory_list.hpp"

#include <list>

namespace px {

	class environment
	{
	public:
		void step(point2 const& direction)
		{
			auto * transform = m_player->transform();
			if (!transform) return;

			point2 destination = transform->position() + direction;

			if (!m_map.traversable(destination)) return;

			transform_component * blocking = nullptr;
			transform->world()->find(destination.x(), destination.y(), [&](int /*x*/, int /*y*/, auto * target) {
				blocking = target;
			});

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
		}

		template <typename Document>
		void add_sprite_atlas(Document && atlas, bool reverse_y)
		{
			m_sprites.add_atlas(std::forward<Document>(atlas), reverse_y);
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

			m_player = &spawn("@", { 54, 46 });

			spawn("m", { 55, 46 });
			spawn("x", { 53, 47 });
			spawn("m_snake", { 50, 50 });

			spawn("p_table", { 55, 47 });
			spawn("p_bookshelf", { 54, 47 });
			spawn("p_locker", { 55, 48 });
			spawn("p_box", { 55, 49 });

			// ui
			m_inventory->set_container(m_player->transform()->linked<body_component>()->linked<container_component>());
		}
		void write(perception & view) const
		{
			// render sprites
			if (m_player)
			{
				auto * transform = m_player->transform();

				float x_offset = -static_cast<float>(transform->x());
				float y_offset = -static_cast<float>(transform->y());
				m_sprites.write(view.batches(), x_offset, y_offset);
			}

			// render user interface
			m_ui.draw(view.canvas());
		}
		unit & spawn(std::string const& name, point2 location)
		{
			m_units.emplace_back();
			unit & result = m_units.back();

			// create

			auto sprite = m_sprites.make_shared(name);
			auto transform = m_transforms.make_shared(location);
			auto body = m_bodies.make_shared();
			auto container = m_containers.make_shared();

			// setup

			for (int i = 0; i != 100; ++i)
			{
				auto itm = std::make_shared<rl::item>();
				itm->set_name("item #" + std::to_string(i));
				itm->add({ rl::effect::ore_power, 0x100, 0x500 });
				container->add(itm);
			}

			body->health().create();

			// assemble

			sprite->connect(transform.get());
			transform->connect(body.get());
			body->connect(container.get());

			result.add(sprite);
			result.add(transform);
			result.add(body);
			result.add(container);

			result.enable();

			return result;
		}
		ui::panel & ui() noexcept
		{
			return m_ui;
		}
		ui::panel const& ui() const noexcept
		{
			return m_ui;
		}
		void layout_ui(rectangle bounds) noexcept
		{
			m_ui.layout(bounds);
		}

	public:
		environment()
		{
			setup_ui();
		}

	private:
		void setup_ui()
		{
			auto inventory_toggle = m_ui.make<ui::toggle>("inventory_toggle", { { 0.25, 0.25 },{ 0, 0 },{ 0, 1 },{ 0.5, 0.0 } });
			inventory_toggle->add_background({ 0, 0, 0.5, 1 });
			inventory_toggle->add_label("Inventory");

			auto inventory_block = m_ui.make<ui::panel>("inventory_block", { { 0.25, 0.25 },{ 0, 1 },{ 0, -1 },{ 0.5, 0.5 } });
			inventory_block->make<ui::board>("background", ui::fill, color{ 0, 0, 1, 1 });

			inventory_toggle->assign_content(inventory_block, false);

			m_inventory = inventory_block->make<ui::inventory_list>("list", ui::fill).get();
		}


	private:
		// systems
		es::sprite_system m_sprites;
		es::transform_system m_transforms;
		es::body_system m_bodies;
		es::container_system m_containers;

		// unit hierarchy
		unit * m_player;
		std::list<unit> m_units;

		// terrain
		map_chunk<tile> m_map;

		// user interface
		ui::panel m_ui;
		ui::inventory_list * m_inventory;
	};
}