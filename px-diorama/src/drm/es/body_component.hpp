// name: body_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>
#include <px/rl/body.hpp>

#include "useable.hpp"

namespace px {

	class transform_component;
	class container_component;
	class character_component;

	class body_component final
		: public es::component
		, public es::link_dispatcher<body_component>
		, public es::link<transform_component>
		, public es::link<container_component>
		, public es::link<character_component>
		, public rl::body
	{
	public:
		void assign_useable(useable * use) noexcept
		{
			m_useable = use;
		}
		void detach_useable() noexcept
		{
			m_useable = nullptr;
		}
		bool can_use(body_component const* user, environment const& shell) const
		{
			return m_useable && m_useable->can_use(user, shell);
		}
		bool try_use(body_component * user, environment & shell)
		{
			return m_useable && m_useable->try_use(user, shell);
		}

	public:
		virtual ~body_component()
		{
			detach_useable();
			clear_body();
		}
		body_component()
			: m_useable(nullptr)
		{
			clear_body();
		}

	private:
		useable * m_useable;
	};

}