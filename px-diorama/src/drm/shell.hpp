// name: shell.hpp
// type: c++ header
// auth: is0urce
// desc: input manager facade and application level event dispatcher

#pragma once

#include "key_translator.hpp"
#include "perception.hpp"
#include "es/transform_system.hpp"
#include "es/sprite_system.hpp"

#include <px/common/matrix.hpp>
#include <px/es/component_collection.hpp>
#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>
#include <px/fn/bsp.hpp>

namespace px
{
	struct tile
	{
		shared_ptr<transform_component> transform;
		shared_ptr<sprite_component> sprite;
		rl::mass<rl::traverse> mass;
	};

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
			m_transform->move(direction);
		}
		void use(unsigned int /*index*/)
		{

		}
		void activate(unsigned int /*mod*/)
		{

		}

		template <typename Document>
		void load_texture(Document const& doc)
		{
			m_perception.add_texture();
			m_sprites.add_texture(doc, true);
		}

		perception const& view() const noexcept
		{
			return m_perception;
		}
		void start()
		{
			auto sprite = m_sprites.make_shared("@");
			auto transform = m_transforms.make_shared({ 0, 0 });
			m_transform = transform.get();

			sprite->assign(transform.get());

			m_player.add(sprite);
			m_player.add(transform);
			m_player.activate();

			m_map.resize({ 100, 100 });
			m_map.enumerate([this](auto const& point, auto & tile) {
				tile.transform = m_transforms.make_shared(point);
				tile.sprite = m_sprites.make_shared("#");

				tile.sprite->assign<transform_component>(tile.transform.get());

				tile.transform->activate();
				tile.sprite->activate();

				tile.mass.make_ground();
			});

			std::mt19937 rng;
			fn::bsp<>::create<>(rng, { { 0,0 },{ 100, 100 } }, 5, 1).enumerate_bounds([&](auto const& room) {
				room.enumerate([&](auto const& point) {
					auto & tile = m_map[point];

					tile.sprite = m_sprites.make_shared(".");
					tile.sprite->assign<transform_component>(tile.transform.get());
					tile.sprite->activate();
				});
			});
		}
		void frame(double /*time*/)
		{
			m_perception.clear();

			float x_offset = -static_cast<float>(m_transform->x());
			float y_offset = -static_cast<float>(m_transform->y());
			m_sprites.write(m_perception.batches(), x_offset, y_offset);
		}
	public:
		shell()
		{
			m_perception.scale(-0.95f);
		}

	private:
		perception m_perception;

		sprite_system m_sprites;
		transform_system m_transforms;

		es::component_collection m_player;
		transform_component * m_transform;
		matrix2<tile> m_map;

		point2 m_hover;
	};
}