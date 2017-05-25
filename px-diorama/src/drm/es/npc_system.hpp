// name: npc_system.hpp
// type: c++
// auth: is0urce
// desc: class

#pragma once

#include <px/es/basic_system.hpp>
#include "npc_component.hpp"

namespace px {
	namespace es {

		class npc_system final
			: public basic_system<npc_component>
		{
		public:
			void fixed_update(unsigned int turns)
			{
				for (unsigned int i = 0; i != turns; ++i) {
					enumerate([](npc_component & npc) {
						npc.act();
					});
				}
			}
		};
	}
}