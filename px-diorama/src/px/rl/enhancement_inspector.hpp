// name: enhancement_inspector.hpp
// type: c++ header
// desc: enum
// auth: is0urce

// status types

#pragma once

#include <px/rl/enhancement.hpp>
#include <px/rl/enhancement_description.hpp>

#include <functional>
#include <optional>
#include <string>
#include <map>

namespace px {
	namespace rl {

		template <typename Effect>
		class enhancement_inspector
		{
		public:
			typedef Effect effect_type;

			typedef std::function<std::string(int, int, double, double)> value_fn;
			typedef std::function<enhancement_status(int, int, double, double)> status_fn;
			typedef std::tuple<std::string, value_fn, status_fn> grinder_type;

		public:
			void expose_hidden() noexcept
			{
				m_honor = false;
			}
			void hide_hidden() noexcept
			{
				m_honor = false;
			}
			void set_honor(bool honor) noexcept
			{
				m_honor = honor;
			}
			void add(effect_type main_type, std::string name, value_fn value, status_fn status)
			{
				m_map[main_type] = { name, value, status };
			}
			std::optional<rl::enhancement_description> inspect(rl::enhancement<effect_type> const& enhance) const
			{
				std::optional<rl::enhancement_description> result;

				if (!enhance.hidden && !m_honor) {
					auto it = m_map.find(enhance.main_type);
					if (it != m_map.end()) {

						result.emplace();

						result->name = std::get<0>(*it);
						result->value = std::get<1>(*it)(enhance.value0, enhance.value1, enhance.magnitude0, enhance.magnitude1);
						result->status = std::get<1>(*it)(enhance.value0, enhance.value1, enhance.magnitude0, enhance.magnitude1);
					}
				}

				return result;
			}

		public:
			enhancement_inspector()
				: m_honor(true)
			{
			}

		private:
			std::map<effect_type, grinder_type> m_map;
			bool m_honor; // do not show hidden enhancements (alternative for debug etc)
		};
	}
}