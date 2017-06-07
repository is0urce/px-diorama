// name: item_functional.cpp

#pragma once

#include <string>

namespace px {
	namespace ui {

		template <typename List>
		class item_transfer
		{
		public:
			template <typename Item>
			void operator()(Item & item)
			{
				auto * from = source->assigned_container();
				auto * to = destination->assigned_container();
				if (from && to) from->transfer(item, *to);
			}
			item_transfer(List from, List to)
				: source(from)
				, destination(to)
			{
			}
			List source;
			List destination;
		};

		struct item_name
		{
			template <typename Item>
			std::string operator()(Item & item)
			{
				return format(std::forward<Item>(item));
			}

			template <typename Item>
			static std::string format(Item & item)
			{
				std::string result;
				if (item) {
					result = item->name();
					auto count = item->count();
					if (count != 1) {
						result += " x" + std::to_string(count);
					}
				}
				else {
					result = "empty";
				}
				return result;
			}
		};
	}
}