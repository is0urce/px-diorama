// name: body_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

#include "drm/rl/body.hpp"

namespace px {

		class body_component final
			: public rl::body
			, public es::component
			, public es::link_dispatcher<body_component>
		{
		public:
			body_component()
			{
			}
			virtual ~body_component()
			{
				clear();
			}

		protected:
			virtual void activate_component() override
			{
			}
			virtual void deactivate_component() override
			{
			}
		};

}