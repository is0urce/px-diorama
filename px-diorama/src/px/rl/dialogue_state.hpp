// name: dialogue_state.hpp
// type: c++ header
// desc: template class
// auth: is0urce

// incapsulates current dialogue state
// created by dialogue::start
// works wery alike to state machine (except input uses visible answers index and shifted accordingly)
// state contains current line-node of gialogue along with user data as StateData
// current possible answers has cashed in visiblility array
// answers availability determined by gialogue_evaluator std::function delegate passed on creation
// answers array is cashed, so answers not flicker at runtime
// to recalculate visibility if needed, recalculate_visibility provided as public method
// to (mutate) move to next state call answer(n) or answer_first()
// to pc-to-npc taks, flow is answer(n) for pc followed by answer_first() for npc
// both methods can additionaly call action std::function delegate in gialogue_evaluator
// all nodes returned are const qualified, as they can be shared by multiple running states of same dialogue

#pragma once

#include <px/rl/dialogue_evaluator.hpp>

#include <vector>

namespace px
{
	namespace rl
	{
		template <typename Node> class dialogue_reply;

		template <typename StateData, typename Node>
		class dialogue_state
		{
		public:

			// query data

			dialogue_reply<Node> const* current() const noexcept
			{
				return m_current;
			}
			StateData const* data() const noexcept
			{
				return m_data;
			}
			StateData* data() noexcept
			{
				return &m_data;
			}

			// query props

			bool contains(size_t index) const noexcept
			{
				return index < m_visible.size();
			}
			bool is_visible(size_t index) const noexcept
			{
				return contains(index) ? m_visible[index] : false;
			}
			size_t total() const noexcept
			{
				return m_visible.size();
			}
			size_t visible() const noexcept
			{
				return m_active;
			}
			bool is_final() const noexcept
			{
				return m_current->is_final();
			}

			// mutate

			// uses reply by visible #, skipping invisible variants
			void answer(size_t variant)
			{
				size_t index = 0; // current
				size_t count = 0; // current visible
				bool visible = m_visible[index];
				while ((!visible || count != variant) && contains(index))
				{
					if (visible) ++count;
					++index;
					visible = m_visible[index];
				}
				answer_absolute(index);
			}

			// uses first visible reply option (same as select(0))
			void answer_first()
			{
				for (size_t index = 0, size = m_current->size(); index != size; ++index)
				{
					if (m_visible[index])
					{
						answer_absolute(index);
						return;
					}
				}
			}

			// aux

			void recalculate_visible()
			{
				m_visible.resize(m_current->size(), false);

				m_active = 0;
				for (size_t index = 0, size = m_visible.size(); index != size; ++index)
				{
					if (m_visible[index] = m_script.conditional(*(*m_current)[index], m_data)) // store to array
					{
						++m_active; // increment cashed counter
					}
				}
			}

			// enumerate by void(size_t visible_index, dialogue_reply<Node> const&)
			template <typename Operator>
			void enumerate(Operator &&fn) const
			{
				size_t visible_index = 0; // visible index
				for (size_t index = 0, size = m_visible.size(); index != size; ++index)
				{
					if (m_visible[index])
					{
						std::forward<Operator>(fn)(visible_index, *(*m_current)[index]);
						++visible_index;
					}
				}
			}

		public:
			dialogue_state(dialogue_reply<Node> const* start, StateData data, dialogue_evaluator<Node, StateData> evaluator)
				: m_data(data)
				, m_current(start)
				, m_script(evaluator)
			{
				recalculate_visible();
			}

		private:
			void answer_absolute(size_t index)
			{
				if (contains(index))
				{
					m_current = (*m_current)[index];
					m_script.action(*m_current, m_data);
					recalculate_visible();
				}
			}

		private:
			dialogue_reply<Node> const* m_current;
			std::vector<bool> m_visible; // visibility array
			size_t m_active; // total visible answers

			StateData m_data;
			dialogue_evaluator<Node, StateData> m_script;
		};
	}
}