// name: effect.hpp
// type: c++ header
// desc: functions
// auth: is0urce

// functions for enum

#pragma once

#include "effect.hpp"

#include <string>
#include <stdexcept>

namespace px {
	namespace rl {

		effect to_effect(std::string const& name) {
			if (name == "weapon_damage") return effect::weapon_damage;
			if (name == "critical") return effect::critical;

			throw std::runtime_error("px::rl::to_effect(string name) error, name=" + name);
		}
	}
}