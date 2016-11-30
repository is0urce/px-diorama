// name: shell.hpp
// type: c++ header
// auth: is0urce
// desc: input manager facade and application level event dispatcher

#pragma once

#include "key.hpp"
#include "perception.hpp"
#include "es/sprite_system.hpp"
#include "es/transform_system.hpp"

#include <px/es/component_collection.hpp>

namespace px
{
	class shell final
	{
	public:
		void press(key /*action*/)
		{

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
	public:
		shell()
		{
			auto sprite = m_sprites.make_shared("@");
			auto transform = m_transforms.make_shared({ 0, 0 });

			sprite->assign(transform.get());

			m_player.add(sprite);
			m_player.add(transform);
		}

	private:
		perception m_perception;
		sprite_system m_sprites;
		transform_system m_transforms;
		es::component_collection m_player;
		int hover_x;
		int hover_y;
	};
}