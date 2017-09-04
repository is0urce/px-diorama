// name: equipment_slot_ext.hpp
// type: c++ header
// desc: function
// auth: is0urce

// equipment slot enum extensions

#pragma once

#include "equipment_slot.hpp"

#include <stdexcept>
#include <string>

namespace px {
	namespace rl {

		equipment_slot to_equipment_slot(std::string const& name) {
			if (name == "weapon_main") return equipment_slot::weapon_main;

			throw std::runtime_error("px::rl::to_equipment_slot(string name) error, name=" + name);
		}
	}
}