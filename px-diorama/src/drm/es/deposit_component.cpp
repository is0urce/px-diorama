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

	static const int popup_color = 0x00;
	static const float popup_size = 1.0f;

	deposit_component::~deposit_component()
	{
	}

	deposit_component::deposit_component()
	{
	}

	bool deposit_component::is_useable(body_component const* user, environment const& /* shell */) const
	{
		auto user_pawn = user ? user->linked<transform_component>() : nullptr;
		auto this_pawn = linked<container_component, transform_component>();

		return user_pawn && this_pawn
			&& point2::king_distance(user_pawn->position(), this_pawn->position()) <= 1;
	}

	void deposit_component::use_useable(body_component * user, environment & shell)
	{
		transform_component * transform = user ? user->linked<transform_component>() : nullptr;
		container_component * container = linked<container_component>();

		std::string popup_text;

		if (!container->empty()) {
			container->enumerate([&popup_text](auto const& item) {
				if (item) {
					popup_text += " + " + item->name();
					auto stack_size = item->count();
					if (stack_size != 1) {
						popup_text += " x" + std::to_string(stack_size);
					}
				}
			});

			shell.loot(user, container);
		}
		else {
			popup_text = "depleted";
		}

		// popup
		if (transform) {
			shell.popup(transform->position(), popup_text, popup_color, popup_size);
		}
	}
}