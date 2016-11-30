// name: shell.hpp
// type: c++ header
// auth: is0urce
// desc: input manager facade and application level event dispatcher

#pragma once

#include "key.hpp"
#include "perception.hpp"
#include "es/transform_system.hpp"
#include "es/sprite_system.hpp"

#include <px/es/component_collection.hpp>

namespace px
{
	class shell final
	{
	public:
		void press(key action)
		{
			if (action == key::move_south)
			{
				m_transform->current.move_axis<1>(-1);
			}
		}
		void text(unsigned int /*codepoint*/)
		{

		}
		void click(int /*button*/)
		{

		}
		void hover(int x, int y)
		{
			hover_x = x;
			hover_y = y;
		}
		void scroll(double /*vertical*/, double /*horisontal*/)
		{

		}
		void frame(double /*time*/)
		{
			for (size_t i = 0, size = m_perception.batches(); i != size; ++i)
			{
				m_sprites.update(m_perception.batch(i));
			}
		}

		template <typename Document>
		void load_texture(Document const& doc)
		{
			m_perception.add_texture();
			m_sprites.add_texture(doc, 0);
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
			sprite->assign(m_transform);

			m_player.add(sprite);
			m_player.add(transform);
			m_player.activate();
		}
	public:
		shell()
		{
		}

	private:
		perception m_perception;
		sprite_system m_sprites;
		transform_system m_transforms;
		transform_component* m_transform;
		es::component_collection m_player;
		int hover_x;
		int hover_y;
	};
}