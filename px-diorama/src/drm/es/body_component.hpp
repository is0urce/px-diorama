// name: body_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>
#include <px/rl/body.hpp>
#include <px/rl/buff.hpp>
#include <px/rl/effect.hpp>

#include "useable.hpp"

#include <functional>
#include <vector>

namespace px {

	class environment;

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
		typedef rl::buff<rl::effect, std::function<void(body_component &, environment &)>> buff_type;

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

		std::vector<buff_type> & buffs() noexcept
		{
			return m_buffs;
		}
		std::vector<buff_type> const& buffs() const noexcept
		{
			return m_buffs;
		}

		template <typename Archive>
		void serialize(Archive & archive)
		{
			archive(static_cast<rl::body &>(*this));
			archive(m_buffs);
		}

	public:
		virtual ~body_component()
		{
			detach_useable();
			clear_body();
			m_buffs.clear();
		}
		body_component()
			: m_useable(nullptr)
		{
			clear_body();
			m_buffs.clear();
		}

	private:
		useable *				m_useable;	// polymorphic link to useable component
		std::vector<buff_type>	m_buffs;	// buffs array
	};

}