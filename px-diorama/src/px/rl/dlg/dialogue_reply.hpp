// name: dialogue_reply.hpp
// type: c++ header
// desc: template class
// auth: is0urce

#pragma once

#include <vector>

namespace px
{
	namespace rl
	{
		template <typename Node>
		class dialogue_reply
		{
		public:
			typedef std::vector<dialogue_reply const*> container_type;

		public:
			bool contains(size_t index) const noexcept
			{
				return index < m_answers.size();
			}
			void link(dialogue_reply* answer)
			{
				m_answers.push_back(answer);
			}
			void link(container_type const& answers)
			{
				auto current = m_answers.size();
				m_answers.resize(answers.size() + current);

				std::copy(std::begin(answers), std::end(answers), std::begin(m_answers) + current);
			}
			size_t size() const noexcept
			{
				return m_answers.size();
			}
			bool is_final() const noexcept
			{
				return m_answers.size() == 0;
			}

			Node const* node() const noexcept
			{
				return &m_data;
			}
			dialogue_reply const* select(size_t n) const noexcept
			{
				return contains(n) ? m_answers[n] : nullptr;
			}
			dialogue_reply const* operator[](size_t n) const noexcept
			{
				return contains(n) ? m_answers[n] : nullptr;
			}

		public:
			dialogue_reply(Node&& node) : m_data(std::forward<Node>(node))
			{
			}

		private:
			Node m_data;
			container_type m_answers;
		};
	}
}