// name: inventory.hpp
// type: c++ header
// desc: class
// auth: is0urce

#pragma once

#include <px/rl/loot/basic_inventory.hpp>
#include <px/rl/loot/item.hpp>

namespace px {
	namespace rl {

		class inventory
			: public basic_inventory<item>
		{
		};
	}
}