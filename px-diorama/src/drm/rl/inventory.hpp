// name: inventory.hpp
// type: c++ header
// desc: template class definition
// auth: is0urce

#pragma once

#include <px/rl/loot/basic_inventory.hpp>
#include <px/rl/loot/basic_item.hpp>

namespace px {
	namespace rl {

		class item
			: public basic_item<effect>
		{
		public:
			unsigned int ORE_POWER;
		};

		class inventory
			: public basic_inventory<item>
		{
		};
	}
}