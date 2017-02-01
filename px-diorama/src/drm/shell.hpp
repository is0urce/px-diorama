// name: shell.hpp
// type: c++ header
// auth: is0urce
// desc: input manager facade and application level event dispatcher

#pragma once

#include "key_translator.hpp"
#include "perception.hpp"

#include "es/transform_system.hpp"
#include "es/sprite_system.hpp"
#include "es/unit.hpp"

#include "rl/map_chunk.hpp"
#include "fn/generator.hpp"

#include <px/es/component_collection.hpp>
#include <px/fn/bsp.hpp>
#include <px/ui/canvas.hpp>
#include <px/ui/panel.hpp>

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
		void add_atlas(Document && doc)
		{
			m_perception.add_texture();
			m_sprites.add_texture(std::forward<Document>(doc), true);
		}

		perception const& view() const noexcept
		{
			return m_perception;
		}
		void start()
		{
			m_player = &spawn("@", { 54, 46 });
			m_player->enable();

			spawn("m", { 55, 46 }).enable();
			spawn("x", { 53, 47 }).enable();
			spawn("m_snake", { 50, 50 }).enable();

			spawn("p_table", { 55, 47 }).enable();
			spawn("p_bookshelf", { 54, 47 }).enable();
			spawn("p_locker", { 55, 48 }).enable();
			spawn("p_box", { 55, 49 }).enable();

			m_map.resize({ 100, 100 });
			m_map.enumerate([this](auto const& point, auto & tile) {
				tile.transform = m_transforms.make_shared(point);

				tile.sprite = m_sprites.make_shared("#");
				tile.sprite->connect(tile.transform.get());
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

				tile.sprite = m_sprites.make_shared(".");
				tile.sprite->connect(tile.transform.get());
				tile.sprite->activate();

				tile.make_ground();
			});
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

			m_canvas.cls();
			m_ui.layout(rectangle(point2(0, 0), m_canvas.range()));
			m_ui.draw(m_canvas);
		}
		unit & spawn(std::string const& name, point2 location)
		{
			unit result;

			auto sprite = m_sprites.make_shared(name);
			auto transform = m_transforms.make_shared(location);

			sprite->connect(transform.get());

			result.add(sprite);
			result.add(transform);

			m_units.push_back(result);

			return m_units.back();
		}

		ui::canvas & canvas()
		{
			return m_canvas;
		}

	public:
		shell()
		{
			m_perception.scale(-0.95f);

			m_ui.make<ui::panel>({ { 0.5, 0.5}, {0, 0}, {0, 0}, {0.5, 0.5} });
		}

	private:
		point2 m_hover;

		sprite_system m_sprites;
		transform_system m_transforms;

		perception m_perception;

		unit * m_player;
		std::list<unit> m_units;

		map_chunk<tile> m_map;

		ui::canvas m_canvas;
		ui::panel m_ui;
	};
}