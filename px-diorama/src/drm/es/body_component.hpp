// name: body_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

#include "drm/rl/body.hpp"

#include "useable.hpp"

namespace px {

	class container_component;

	class body_component final
		: public rl::body
		, public es::component
		, public es::link_dispatcher<body_component>
		, public es::link<container_component>
	{
	public:
		void assign_useable(useable * use)
		{
			m_useable = use;
		}
		void clear_useable() noexcept
		{
			m_useable = nullptr;
		}
		bool can_use(body_component const& body, environment const& shell) const
		{
			return m_useable && m_useable->can_use(body, shell);
		}
		bool use(body_component & body, environment & shell)
		{
			return m_useable && m_useable->use(body, shell);
		}

	public:
		virtual ~body_component()
		{
			clear_useable();
			clear_body();
		}
		body_component()
			: m_useable(nullptr)
		{
		}

	private:
		useable * m_useable;
	};

}