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
		void use(body_component & body)
		{
			if (can_use(body))
			{
				use_useable(body);
			}
		}
	protected:
		virtual bool is_useable(body_component const& /*body*/) const
		{
			return false;
		}
		virtual bool use_useable(body_component & /*body*/) = 0
		{

		}
	};
}