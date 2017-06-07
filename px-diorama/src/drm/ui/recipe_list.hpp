// name: recipe_list.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/common/enumerable.hpp>

#include "drm/rl/kraft/recipe.hpp"

#include <list>

namespace px {
	namespace ui {

		//class recipe_list
		//	: public std::list<rl::recipe>
		//	, public enumerator<std::list<rl::recipe>>
		//{
		//public:
		//	recipe_list(std::list<rl::recipe> list)
		//		: enumerator(static_cast<std::list<rl::recipe>&>(*this))
		//		, std::list<rl::recipe>(list)
		//	{
		//	}
		//	recipe_list(std::list<rl::recipe> && list)
		//		: std::list<rl::recipe>(std::forward<std::list<rl::recipe>>(list))
		//		, enumerator(static_cast<std::list<rl::recipe>&>(*this))
		//	{
		//	}
		//	recipe_list()
		//		: enumerator(static_cast<std::list<rl::recipe>&>(*this))
		//	{
		//	}
		//};

		class recipe_list : public enumerable<std::list<rl::recipe>> {};
	}
}