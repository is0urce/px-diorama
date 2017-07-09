// name: qtree.hpp
// type: c++ header
// desc: class
// auth: is0urce

// quad tree space partition
// template with element type
// multiple elements can occupy same spot

#pragma once

#include <cmath> // abs
#include <memory>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

//#include <px/common/point.hpp>

namespace px {

	template <typename T>
	class qtree
	{
	public:
		typedef T element_type;
		typedef std::unique_ptr<qtree> ptr;
		struct bucket;

	public:
		// this partition contains point in range (there could be no elements)
		bool covers(int x, int y) const noexcept
		{
			return x >= m_center_x - static_cast<int>(m_range)
				&& x < m_center_x + static_cast<int>(m_range)
				&& y >= m_center_y - static_cast<int>(m_range)
				&& y < m_center_y + static_cast<int>(m_range);
		}
		bool leaf() const noexcept
		{
			return m_bucket;
		}

		void add(int x, int y, element_type e)
		{
			while (!covers(x, y)) {
				expand();
			}
			insert(x, y, e);
		}
		void remove(int x, int y, element_type e)
		{
			erase(x, y, e);
		}

		// Operator sould support bool operator()(int, int, T)
		template <typename Operator>
		void find(int x, int y, unsigned int radius, Operator && fn) const
		{
			if (m_bucket)
			{
				if (std::abs(x - m_bucket->x()) <= static_cast<int>(radius) && std::abs(y - m_bucket->y()) <= static_cast<int>(radius))
				{
					m_bucket->enumerate(fn);
				}
			}
			else
			{
				bool w = x - static_cast<int>(radius) <= m_center_x;
				bool e = x + static_cast<int>(radius) >= m_center_x;
				bool n = y + static_cast<int>(radius) >= m_center_y;
				bool s = y - static_cast<int>(radius) <= m_center_y;
				if (n && w && nw) nw->find(x, y, radius, fn);
				if (n && e && ne) ne->find(x, y, radius, fn);
				if (s && w && sw) sw->find(x, y, radius, fn);
				if (s && e && se) se->find(x, y, radius, fn);
			}
		}

		// Operator sould support bool operator()(int, int, T)
		template <typename Operator>
		void find(int x, int y, Operator && fn) const
		{
			if (m_bucket && m_bucket->match(x, y))
			{
				m_bucket->enumerate(fn);
			}
			else
			{
				auto const& branch = select(x, y);
				if (branch) branch->find(x, y, std::forward<Operator>(fn));
			}
		}
		bool exists(int x, int y) const
		{
			if (m_bucket)
			{
				return !m_bucket->empty() && m_bucket->match(x, y);
			}
			else
			{
				auto const& branch = select(x, y);
				return branch ? branch->exists(x, y) : false;
			}
		}
		void move(int sx, int sy, element_type e, int dx, int dy)
		{
			move_hint(sx, sy, e, dx, dy)->add(dx, dy, e);
		}

		std::string to_string() const
		{
			auto result = std::string("qT(") + std::to_string(m_center_x) + std::string(",") + std::to_string(m_center_y) + std::string(")-") + std::to_string(m_range);
			if (m_bucket) result += std::string(":") + std::to_string(m_bucket->size());
			if (nw) result += std::string(" nw=") + nw->to_string();
			if (ne) result += std::string(" ne=") + ne->to_string();
			if (sw) result += std::string(" sw=") + sw->to_string();
			if (se) result += std::string(" se=") + se->to_string();
			result += ";";
			return result;
		}

	public:
		qtree(int x, int y, unsigned int range)
			: m_center_x(x)
			, m_center_y(y)
			, m_range(range)
			, m_bucket(std::make_unique<bucket>())
		{
		}
		qtree(unsigned int range)
			: qtree(0, 0, range)
		{
		}
		qtree(qtree const&) = delete;
		qtree& operator=(qtree const&) = delete;

	private:
		// select branch for specified coordinates
		// internal garantee coordinates are in quad
		ptr & select(int x, int y) noexcept
		{
			if (x >= m_center_x) {
				return y >= m_center_y ? ne : se;
			}
			else {
				return y >= m_center_y ? nw : sw;
			}
		}
		ptr const& select(int x, int y) const noexcept
		{
			if (x >= m_center_x) {
				return y >= m_center_y ? ne : se;
			}
			else {
				return y >= m_center_y ? nw : sw;
			}
		}

		// select branch, if none available, create one
		// souldn't used by itself on a leaves as it interfere with hierarchy leaves detection
		ptr & access(int x, int y)
		{
			auto & branch = select(x, y);
			if (!branch)
			{
				int range = m_range / 2;
				auto result = std::make_unique<qtree>(m_center_x + ((x >= m_center_x) ? range : -range), m_center_y + ((y >= m_center_y) ? range : -range), range);
				std::swap(branch, result);
			}
			return branch;
		}

		// partition current leaf
		void split()
		{
			if (!m_bucket) throw std::runtime_error("px::qtree:split() - can't split non-leaf");

			auto &branch = access(m_bucket->x(), m_bucket->y());
			std::swap(m_bucket, branch->m_bucket);
			m_bucket.release();
		}

		// empty or not-created leaf
		bool static is_empty(ptr const& tree) noexcept
		{
			return !tree || tree && tree->m_bucket && tree->m_bucket->empty();
		}

		// opposite command to split
		// combine leaves back
		// all branches must be leaves
		// only one branch can have elements - collapse it and put that branch on top
		bool combine(ptr & cleaned)
		{
			if (!cleaned->m_bucket) throw std::runtime_error("can't collapse at non-leaf");

			bool enw = is_empty(nw);
			bool ene = is_empty(ne);
			bool esw = is_empty(sw);
			bool ese = is_empty(se);

			if (nw && nw->m_bucket && ene && esw && ese)
			{
				std::swap(m_bucket, nw->m_bucket);
				nw.reset();
				return true;
			}
			if (ne && ne->m_bucket && enw && esw && ese)
			{
				std::swap(m_bucket, ne->m_bucket);
				ne.reset();
				return true;
			}
			if (sw && sw->m_bucket && ene && enw && ese)
			{
				std::swap(m_bucket, sw->m_bucket);
				sw.reset();
				return true;
			}
			if (se && se->m_bucket && ene && esw && enw)
			{
				std::swap(m_bucket, se->m_bucket);
				se.reset();
				return true;
			}
			return false;
		}

		// helper function for expansion into
		// old - part for expanding (internal not null garantee)
		// expansion - bigger area, not null
		// update - area in bigger area for swap to old, is null
		void static expand(ptr & old, ptr & expansion, ptr & update)
		{
			expansion->m_bucket.reset();
			if (old->m_bucket)
			{
				std::swap(old->m_bucket, expansion->m_bucket);
				old->m_bucket.reset();
			}
			else
			{
				std::swap(update, old);
			}
			std::swap(old, expansion);
		}

		// expand range
		void expand()
		{
			int range = static_cast<int>(m_range);
			if (nw)
			{
				auto expand = std::make_unique<qtree>(m_center_x - range, m_center_y + range, range);
				qtree::expand(nw, expand, expand->se);
			}
			if (ne)
			{
				auto expand = std::make_unique<qtree>(m_center_x + range, m_center_y + range, range);
				qtree::expand(ne, expand, expand->sw);
			}
			if (sw)
			{
				auto expand = std::make_unique<qtree>(m_center_x - range, m_center_y - range, range);
				qtree::expand(sw, expand, expand->ne);
			}
			if (se)
			{
				auto expand = std::make_unique<qtree>(m_center_x + range, m_center_y - range, range);
				qtree::expand(se, expand, expand->nw);
			}
			m_range *= 2;
		}

		// adding element, internal guarantee x and y in range
		void insert(int x, int y, element_type e)
		{
			if (m_bucket) // it is a leaf
			{
				if (m_bucket->empty() || m_bucket->match(x, y))
				{
					m_bucket->add(e, x, y);
				}
				else
				{
					// partition and try again
					split();
					insert(x, y, e);
				}
			}
			else
			{
				auto &branch = access(x, y);
				branch->insert(x, y, e);
			}
		}
		bool erase(int x, int y, element_type e)
		{
			if (m_bucket)
			{
				if (!m_bucket->match(x, y)) throw std::runtime_error("px::qtree::remove - bucket coordinates not match");
				if (!m_bucket->remove(e)) throw std::runtime_error("px::qtree::remove - item not found");

				return true;
			}
			else
			{
				ptr &branch = select(x, y);

				if (!branch) throw std::runtime_error("px::qtree::remove - no branch");

				if (branch->erase(x, y, e))
				{
					return combine(branch); // try to optimize
				}
				return false;
			}
		}
		qtree * move_hint(int sx, int sy, element_type e, int dx, int dy)
		{
			if (m_bucket)
			{
				if (!m_bucket->match(sx, sy)) throw std::runtime_error("px::qtree::move_hint - bucket coordinates not match");
				if (!m_bucket->remove(e)) throw std::runtime_error("px::qtree::move_hint - item not found");

				return this;
			}
			else
			{
				ptr &branch = select(sx, sy);

				if (!branch)
				{
					throw std::runtime_error("px::qtree::move_hint - no branch");
				}

				if (branch->covers(dx, dy))
				{
					return branch->move_hint(sx, sy, e, dx, dy);
				}
				else
				{
					erase(sx, sy, e);
					return this;
				}
			}
		}

	private:
		ptr nw, ne, se, sw;					// leaves
		std::unique_ptr<bucket>	m_bucket;	// value, not null if it is a leaf
		int						m_center_x;
		int						m_center_y;
		unsigned int			m_range;

		struct bucket
		{
		public:
			template <typename CallbackOperator>
			void enumerate(CallbackOperator && fn)
			{
				for (auto & e : m_elements) {
					fn(m_x, m_y, e);
				}
			}
			template <typename CallbackOperator>
			void enumerate_while(CallbackOperator && fn)
			{
				for (auto it = std::begin(m_elements), last = std::end(m_elements); it != last; ++it) {
					if (!fn(m_x, m_y, *it)) return;
				}
			}
			bool match(int x, int y) const noexcept
			{
				return m_x == x && m_y == y;
			}
			size_t size() const noexcept
			{
				return m_elements.size();
			}
			bool empty() const noexcept
			{
				return m_elements.empty();
			}
			void add(element_type e, int x, int y)
			{
				m_elements.push_back(e);
				m_x = x;
				m_y = y;
			}
			bool remove(element_type e)
			{
				for (auto it = std::begin(m_elements), last = std::end(m_elements); it != last; /* increment is conditional */) {
					if (*it == e) {
						std::swap(*it, m_elements.back());
						m_elements.pop_back();
						return true;
					}
					++it;
				}
				return false;
			}
			int x() const noexcept
			{
				return m_x;
			}
			int y() const noexcept
			{
				return m_y;
			}

		private:
			std::vector<element_type> m_elements;
			int m_x;
			int m_y;
		};
	};
}