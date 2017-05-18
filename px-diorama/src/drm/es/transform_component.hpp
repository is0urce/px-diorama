// name: transform_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/common/vector.hpp>

#include <px/es/transform.hpp>
#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

namespace px {

	class body_component;

	class transform_component final
		: public es::transform<transform_component>
		, public es::component
		, public es::link_dispatcher<transform_component>
		, public es::link<body_component>
	{
	public:
		void store_position() noexcept
		{
			m_last = position();
		}
		void set_last_position(point2 last) noexcept
		{
			m_last = last;
		}
		point2 const& last_position() const noexcept
		{
			return m_last;
		}
		vector2 interpolate(double w) const
		{
			return vector2(m_last).lerp(position(), w);
		}

		template <typename Archive>
		void serialize(Archive & archive)
		{
			transform::serialize(archive);
			archive(m_last);
		}

	public:
		virtual ~transform_component()
		{
			retract();
			reincarnate();
		}
		transform_component()
		{
		}
		transform_component(point2 start)
		{
			place(start);
			m_last = start;
		}
		transform_component(point2 start, point2 last)
		{
			place(start);
			m_last = last;
		}

	protected:
		virtual void activate_component() override
		{
			insert();
		}
		virtual void deactivate_component() override
		{
			retract();
		}

	private:
		point2 m_last;
	};
}