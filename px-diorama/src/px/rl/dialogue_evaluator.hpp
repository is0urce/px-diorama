// name: dialogue_evaluator.hpp
// type: c++ header
// desc: template class
// auth: is0urce

#pragma once

#include <functional>

namespace px
{
	namespace rl
	{
		template <typename Node> class dialogue_reply;

		template <typename Node, typename StateData = void*>
		class dialogue_evaluator
		{
		public:
			typedef std::function<bool(dialogue_reply<Node> const&, StateData&)> conditional_fn;
			typedef std::function<void(dialogue_reply<Node> const&, StateData&)> action_fn;

		public:
			bool conditional(dialogue_reply<Node> const& reply, StateData & data) const
			{
				return m_conditional ? m_conditional(reply, data) : true;
			}
			void action(dialogue_reply<Node> const& reply, StateData & data) const
			{
				if (m_action)
				{
					m_action(reply, data);
				}
			}

			void set_conditional(conditional_fn predicate)
			{
				m_conditional = predicate;
			}
			void set_action(action_fn predicate)
			{
				m_action = predicate;
			}

		public:
			dialogue_evaluator()
			{
			}
			dialogue_evaluator(conditional_fn conditional_predicate, action_fn action_predicate)
			{
				m_conditional = conditional_predicate;
				m_action = action_predicate;
			}

		private:
			conditional_fn m_conditional;
			action_fn m_action;
		};
	}
}