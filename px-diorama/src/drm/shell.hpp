// name: shell.hpp
// type: c++ header
// auth: is0urce
// desc: input manager facade and application level event dispatcher

#pragma once

#include "key_translator.hpp"
#include "perception.hpp"
#include "es/transform_system.hpp"
#include "es/sprite_system.hpp"

#include <px/es/component_collection.hpp>

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
			m_perception.scale(static_cast<float>(vertical + horisontal));
		}
		void step(point2 const& direction)
		{
			m_player.component<transform_component>()->move(direction);
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

			sprite->assign(transform.get());

			m_player.add(sprite);
			m_player.add(transform);
			m_player.activate();
		}
		void frame(double /*time*/)
		{
			for (size_t i = 0, size = m_perception.batches(); i != size; ++i)
			{
				m_sprites.update(m_perception.batch(i));
			}
		}
	public:
		shell()
		{
		}

	private:
		perception m_perception;

		sprite_system m_sprites;
		transform_system m_transforms;

		es::component_collection m_player;
		point2 m_hover;
	};
}