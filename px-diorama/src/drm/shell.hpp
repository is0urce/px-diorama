// name: shell.hpp
// type: c++ header
// auth: is0urce
// desc: input manager facade and application level event dispatcher

#pragma once

#include "key_translator.hpp"
#include "perception.hpp"

#include "es/transform_system.hpp"
#include "es/sprite_system.hpp"
#include "es/body_system.hpp"
#include "es/container_system.hpp"
#include "es/unit.hpp"

#include "rl/map_chunk.hpp"
#include "fn/generator.hpp"

#include <px/common/colors.hpp>
#include <px/es/component_collection.hpp>
#include <px/fn/bsp.hpp>
#include <px/ui/canvas.hpp>
#include <px/ui/panel.hpp>
#include <px/ui/board.hpp>

#include "ui/inventory_panel.hpp"

#include <list>

namespace px
{
	class shell
		: public key_translator<shell>
	{
	public:
		void text(unsigned int /*codepoint*/)
		{

		}
		void click(int /*button*/)
		{
			activate(0);
		}
		void hover(int x, int y)
		{
			m_hover = { x, y };
		}
		void scroll(double vertical, double horisontal)
		{
			m_perception.scale(static_cast<float>(vertical + horisontal) * 0.1f);
		}
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
		void add_atlas(Document && atlas)
		{
			m_perception.add_texture();
			m_sprites.add_atlas(std::forward<Document>(atlas), true);
		}

		perception const& view() const noexcept
		{
			return m_perception;
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
		void frame(double /*time*/)
		{
			m_perception.clear();

			if (m_player)
			{
				auto * transform = m_player->transform();

				float x_offset = -static_cast<float>(transform->x());
				float y_offset = -static_cast<float>(transform->y());
				m_sprites.write(m_perception.batches(), x_offset, y_offset);
			}

			// update user interface
			m_perception.canvas().cls();
			m_ui.layout(rectangle(point2(0,0), m_perception.canvas().range()));
			m_ui.draw(m_perception.canvas());
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

			auto itm = std::make_shared<rl::item>();
			itm->set_name("item #X");
			itm->add({ rl::effect::ore_power, 0x100, 0x500 });
			container->add(itm);

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
		void resize_ui(unsigned int width, unsigned int height)
		{
			m_perception.canvas().resize(width, height);
		}

	public:
		shell()
		{
			m_perception.scale(-0.95f);

			m_inventory = m_ui.make<ui::inventory_panel>("inventory", { { 0.25, 0.25}, {0, 0}, {0, 0}, {0.5, 0.5} }).get();
			m_inventory->make<ui::board>("bg", ui::fill, colors::blue);
		}

	private:
		point2 m_hover;

		es::sprite_system m_sprites;
		es::transform_system m_transforms;
		es::body_system m_bodies;
		es::container_system m_containers;

		perception m_perception;

		unit * m_player;
		std::list<unit> m_units;

		map_chunk<tile> m_map;

		ui::panel m_ui;
		ui::inventory_panel * m_inventory;
	};
}