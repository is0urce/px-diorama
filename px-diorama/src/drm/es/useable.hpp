// name: useable.hpp
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

namespace px {

	class body_component;

	class useable
	{
	public:
		bool can_use(body_component const& body) const
		{
			return is_useable(body);
		}
		bool use(body_component & body)
		{
			if (!can_use(body)) return false;

			use_useable(body);
			return true;
		}

	public:
		virtual ~useable()
		{
		}

	protected:
		virtual bool is_useable(body_component const& /*body*/) const
		{
			return false;
		}
		virtual void use_useable(body_component & /*body*/) = 0
		{
		}
	};
}