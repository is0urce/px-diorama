// name: enhancement_collection.hpp
// type: c++ header
// desc: struct
// auth: is0urce

// collection of enhancements - single item or buff

#pragma once

#include <px/rl/enhancement.hpp>

#include <vector>
#include <algorithm>

namespace px
{
	namespace rl
	{
		template <typename Effect>
		class enhancement_collection
		{
		public:
			typedef Effect effect_type;
			typedef enhancement<effect_type> enhancement_type;

		private:
			std::vector<enhancement_type> m_effects;

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
			bool remove_first(effect_type e)
			{
				bool done = false;
				for (auto it = m_effects.begin(), last = m_effects.end(); it != last; ++it)
				{
					if (it->main_type == e)
					{
						m_effects.erase(it);
						done = true;
						break;
					}
				}
				return done;
			}

			// returns true if element removed
			template <typename Predicate>
			bool remove_first(Predicate&& predicate)
			{
				bool done = false;
				for (auto it = m_effects.begin(), last = m_effects.end(); it != last; ++it)
				{
					if (std::forward<Predicate>(predicate)(*it))
					{
						it = m_effects.erase(it);
						done = true;
						break;
					}
				}
				return done;
			}

			// access

			template <typename CallbackOperator>
			void enumerate(CallbackOperator&& fn) const
			{
				for (auto it = m_effects.cbegin(), last = m_effects.cend(); it != last; ++it)
				{
					std::forward<CallbackOperator>(fn)(*it);
				}
			}

			template <typename T, typename BinaryOperation>
			T accumulate(T start, BinaryOperation&& fold) const
			{
				for (auto it = m_effects.cbegin(), last = m_effects.cend(); it != last; ++it)
				{
					start = std::forward<BinaryOperation>(fold)(start, *it);
				}
				return start;
			}

			template <effect_type TValue>
			enhancement_type accumulate() const
			{
				enhancement_type start{};
				for (auto it = m_effects.cbegin(), last = m_effects.cend(); it != last; ++it)
				{
					if (it->main_type == TValue)
					{
						start += *it;
					}
				}
				return start;
			}

			template <effect_type TValue>
			std::pair<bool, enhancement_type> find() const
			{
				for (auto const& e : m_effects)
				{
					if (e.main_type == TValue)
					{
						return{ true, e };
					}
				}
				return{ false, {} };
			}

			// compare

			bool compare(enhancement_collection vs) const
			{
				return std::equal(m_effects.cbegin(), m_effects.cend(), vs.m_effects.cbegin(), vs.m_effects.cend(), [](const enhancement_type &l, const enhancement_type &r) { return l == r; });
			}
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
				return !operator=(lh, rh);
			}
		}
	}
}