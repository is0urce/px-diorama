// name: body_system.hpp
// type: c++ header
// auth: is0urce
// desc: class

#pragma once

#include <px/es/basic_system.hpp>

#include "drm/es/body_component.hpp"

#include <algorithm>

namespace px {
	namespace es {

		class body_system final
			: public basic_system<body_component>
		{
		public:
			void provide_environment(environment * shell) noexcept
			{
				m_environment = shell;
			}
			void fixed_update(unsigned int ticks)
			{
				px_assert(m_environment);

				if (m_environment) {
					for (unsigned int i = 0; i != ticks; ++i) {
						enumerate([this](body_component & component) {
							auto & buffs = component.buffs();
							for (auto & buff : component.buffs()) {
								buff.tick(component, *m_environment);
							}

							buffs.erase(std::remove_if(buffs.begin(), buffs.end(), [](auto const& b) { return b.expired(); }), buffs.end());
						});
					}
				}
			}

		public:
			body_system()
				: m_environment(nullptr)
			{
			}

		private:
			environment * m_environment;
		};
	}
}