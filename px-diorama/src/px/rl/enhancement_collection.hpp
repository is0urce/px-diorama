// name: enhancement_collection.hpp
// type: c++ header
// desc: struct
// auth: is0urce

// collection of enhancements - single item or buff

#pragma once

#include <px/rl/enhancement.hpp>

#include <vector>
#include <algorithm>

namespace px {
	namespace rl {

		template <typename Effect>
		class enhancement_collection
		{
		public:
			typedef Effect effect_type;
			typedef enhancement<effect_type> enhancement_type;

		public:

			// manage

			void clear()
			{
				m_effects.clear();
			}
			void add(enhancement_type e)
			{
				m_effects.push_back(e);
			}

			// returns true if element removed
			bool remove_first(effect_type efx_type)
			{
				bool done = false;
				for (auto it = m_effects.begin(), last = m_effects.end(); it != last; ++it)	{
					if (it->main_type == efx_type) {
						m_effects.erase(it);
						done = true;
						break;
					}
				}
				return done;
			}

			// returns true if element removed
			template <typename Predicate>
			bool remove_first(Predicate && predicate)
			{
				bool done = false;
				for (auto it = m_effects.begin(), last = m_effects.end(); it != last; ++it)	{
					if (predicate(*it))	{
						it = m_effects.erase(it);
						done = true;
						break;
					}
				}
				return done;
			}

			// access

			template <typename CallbackOperator>
			void enumerate(CallbackOperator && fn) const
			{
				for (auto const& efx : m_effects) {
					fn(efx);
				}
			}

			template <typename Accumulator, typename BinaryOperation>
			Accumulator accumulate(Accumulator start, BinaryOperation && fold) const
			{
				for (auto const& efx : m_effects) {
					start = fold(start, efx);
				}
				return start;
			}

			template <effect_type MainType>
			enhancement_type accumulate(enhancement_type accumulator) const
			{
				for (auto const& efx : m_effects) {
					if (efx.main_type == MainType) {
						accumulator += efx;
					}
				}
				return accumulator;
			}

			template <effect_type MainType>
			enhancement_type accumulate() const
			{
				return accumulate<MainType>(enhancement_type{});
			}

			template <effect_type MainType>
			auto value() const
			{
				return accumulate<MainType>(enhancement_type{}).value0;
			}

			template <effect_type MainType>
			std::pair<bool, enhancement_type> find() const
			{
				for (auto const& efx : m_effects) {
					if (efx.main_type == MainType) {
						return{ true, efx };
					}
				}
				return{ false, enhancement_type{} };
			}

			template <effect_type MainType>
			bool has_effect() const
			{
				for (auto const& efx : m_effects) {
					if (efx.main_type == MainType) return true;
				}
				return false;
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(m_effects);
			}

			// compare

			bool compare(enhancement_collection const& vs) const
			{
				return std::equal(m_effects.cbegin(), m_effects.cend(), vs.m_effects.cbegin(), vs.m_effects.cend(), [](enhancement_type const& l, enhancement_type const & r) { return l == r; });
			}


		private:
			std::vector<enhancement_type> m_effects;
		};

		namespace
		{
			template <typename E>
			bool operator==(const enhancement_collection<E>& lh, const enhancement_collection<E>& rh)
			{
				return lh.compare(rh);
			}
			template <typename E>
			bool operator!=(const enhancement_collection<E>& lh, const enhancement_collection<E>& rh)
			{
				return !operator==(lh, rh);
			}
		}
	}
}