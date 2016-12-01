// name: key_adapter.hpp
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

// traslation of px::key actiont to call step, action or activate via CRTP

#include "key.hpp"

namespace px
{
	template <typename Sub>
	class key_translator
	{
	public:
		void press(key code)
		{
			auto & game = static_cast<Sub&>(*this);
			switch (code)
			{
			case key::move_none:
				game.step({ 0, 0 });
				break;
			case key::move_north:
				game.step({ 0, 1 });
				break;
			case key::move_south:
				game.step({ 0, -1 });
				break;
			case key::move_east:
				game.step({ 1, 0 });
				break;
			case key::move_west:
				game.step({ -1, 0 });
				break;
			case key::move_southwest:
				game.step({ -1, -1 });
				break;
			case key::move_southeast:
				game.step({ 1, -1 });
				break;
			case key::move_northwest:
				game.step({ -1, 1 });
				break;
			case key::move_northeast:
				game.step({ 1, 1 });
				break;
			case key::action_use:
				game.activate(0);
				break;
			case key::action_alt:
				game.activate(1);
				break;
			case key::action0:
				game.use(0);
				break;
			case key::action1:
				game.use(1);
				break;
			case key::action2:
				game.use(2);
				break;
			case key::action3:
				game.use(3);
				break;
			case key::action4:
				game.use(4);
				break;
			case key::action5:
				game.use(5);
				break;
			default:
				break;
			}
		}
	};
}