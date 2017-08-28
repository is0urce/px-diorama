// name: editor_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>
#include "drm/environment.hpp"

namespace px {
	namespace ui {

		class editor_panel
			: public panel
		{
		public:
			void assign_environment(environment * environment)
			{
			}

		public:
			virtual ~editor_panel()
			{
			}
			editor_panel()
				: m_environment(nullptr)
			{
			}

		private:
			environment *	m_environment;
		};
	}
}