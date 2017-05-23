// name: item.hpp
// type: c++ header
// desc: class
// auth: is0urce

#pragma once

#include <px/rl/loot/basic_item.hpp>
#include <px/rl/effect.hpp>

namespace px {
	namespace rl {

		class item
			: public basic_item<effect>
		{
		};
	}
}