// name: npc_system.hpp
// type: c++
// auth: is0urce
// desc: class

#pragma once

#include <px/es/basic_system.hpp>
#include "npc_component.hpp"

#include "drm/environment.hpp"

namespace px {
	namespace es {

		class npc_system final
			: public basic_system<npc_component>
		{
		public:
			void provide_environment(environment * shell) noexcept
			{
				m_environment = shell;
			}
			void fixed_update(unsigned int turns)
			{
				px_assert(m_environment);
				if (m_environment) {
					for (unsigned int i = 0; i != turns; ++i) {
						enumerate([this](npc_component & npc) {
							npc.act(*m_environment);
						});
					}
				}
			}

		public:
			npc_system()
				: m_environment(nullptr)
			{
			}

		private:
			environment * m_environment;
		};
	}
}