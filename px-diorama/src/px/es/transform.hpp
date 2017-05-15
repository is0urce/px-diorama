// transform.hpp

#pragma once

#include <px/common/point.hpp>
#include <px/common/qtree.hpp>

namespace px {
	namespace es {

		template <typename Sub>
		class transform
		{
		public:
			typedef qtree<Sub*> space_type;

		public:
			auto const& position() const noexcept
			{
				return m_current;
			}
			auto const& last_position() const noexcept
			{
				return m_last;
			}
			auto x() const noexcept
			{
				return m_current.x();
			}
			auto y() const noexcept
			{
				return m_current.y();
			}
			void move(point2 const& direction)
			{
				place(m_current + direction);
			}
			void place(point2 destination)
			{
				if (m_space && m_inside) {
					m_space->move(x(), y(), static_cast<Sub*>(this), destination.x(), destination.y());
				}
				m_current = destination;
			}
			void store_position() noexcept
			{
				m_last = m_current;
			}

			void incarnate(space_type * space)
			{
				if (m_inside) {
					do_retract();
					m_space = space;
					do_insert();
				}
				else {
					m_space = space;
				}
			}
			void reincarnate()
			{
				incarnate(nullptr);
			}
			void insert()
			{
				if (!m_inside) do_insert();
				m_inside = true;
			}
			void retract()
			{
				if (m_inside) do_retract();
				m_inside = false;
			}

			space_type * world() const noexcept
			{
				return m_space;
			}
			template <typename Operator>
			void nearest(Operator && fn) const
			{
				m_space->find(m_current.x(), m_current.y(), [&](int /* x */, int /* y */, Sub * e) {
					fn(e);
				});
			}
			template <typename Operator>
			void nearest(unsigned int radius, Operator && fn) const
			{
				m_space->find(m_current.x(), m_current.y(), radius, [&](int x, int y, Sub * e) {
					fn(e, point2{ x, y });
				});
			}

			// space is not serizlized
			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(m_current, m_last);
			}

		public:
			transform() noexcept
				: m_inside(false)
				, m_space(nullptr)
			{
			}

		protected:
			void do_insert()
			{
				if (m_space) m_space->add(x(), y(), static_cast<Sub*>(this));
			}
			void do_retract()
			{
				if (m_space) m_space->remove(x(), y(), static_cast<Sub*>(this));
			}

		private:
			space_type *	m_space;
			bool			m_inside;
			point2			m_current;
			point2			m_last;
		};
	}
}