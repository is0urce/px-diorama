// name: inventory.hpp
// type: c++ header
// desc: template class definition
// auth: is0urce

#pragma once

#include <px/rl/lt/basic_inventory.hpp>
#include "item.hpp"

namespace px
{
	namespace rl
	{
		class inventory : public basic_inventory<rl::item<rl::effect>>
		{
		};
	}
}