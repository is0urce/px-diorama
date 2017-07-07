// name: deposit_component.cpp
// type: c++ code
// auth: is0urce
// desc: class methods implementation

#include "deposit_component.hpp"

#include "drm/environment.hpp"

#include "body_component.hpp"
#include "container_component.hpp"
#include "transform_component.hpp"

namespace px {

	deposit_component::~deposit_component()
	{
	}

	deposit_component::deposit_component()
		: m_depleted(false)
	{
	}

	bool deposit_component::is_useable(body_component const* user, environment const& /* shell */) const
	{
		auto user_pawn = user ? user->linked<transform_component>() : nullptr;
		auto this_pawn = linked<container_component, transform_component>();

		return user_pawn && this_pawn
			&& point2::king_distance(user_pawn->position(), this_pawn->position()) <= 1;
	}

	void deposit_component::deplete()
	{
		m_depleted = true;
	}
	bool deposit_component::depleted() const noexcept
	{
		return m_depleted;
	}

	void deposit_component::use_useable(body_component * user, environment & shell)
	{
		transform_component * transform = user ? user->linked<transform_component>() : nullptr;
		container_component * container = linked<container_component>();

		std::string result;

		if (!m_depleted) {
			container->enumerate([&result](auto const& item) {
				if (item) {
					result += " + ";
					result = item->name();
					auto count = item->count();
					if (count != 1) {
						result += " x" + std::to_string(count);
					}
				}
				else {
					result = "empty";
				}
			});

			shell.loot(user, container);
			deplete();
		}
		else {
			result = "depleted";
		}

		// popup
		if (transform) {
			shell.popup(transform->position(), result, 0x00, 1.0f);
		}
	}
}