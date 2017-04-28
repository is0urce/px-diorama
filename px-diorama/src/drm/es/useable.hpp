// name: useable.hpp
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

namespace px {

	class environment;
	class body_component;

	class useable
	{
	public:
		bool can_use(body_component const* user, environment const& shell) const
		{
			return is_useable(user, shell);
		}
		bool try_use(body_component * user_body, environment & shell)
		{
			if (!can_use(user_body, shell)) return false;

			use_useable(user_body, shell);
			return true;
		}

	public:
		virtual ~useable()
		{
		}

	protected:
		virtual bool is_useable(body_component const* /*body*/, environment const& /*shell*/) const
		{
			return false;
		}
		virtual void use_useable(body_component * /*body*/, environment & /*shell*/) = 0
		{
		}
	};
}