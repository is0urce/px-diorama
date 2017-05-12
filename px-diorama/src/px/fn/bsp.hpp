// name: bsp.hpp
// type: c++ header
// desc: template class
// auth: is0urce

#pragma once

// binary space partition generator

#include <px/common/rectangle.hpp>

#include <memory>
#include <random>

namespace px {
	namespace fn {

		// Random - rng device
		// Props - user-defined data associated to node
		template <typename Props = void*>
		class bsp
		{
		public:
			typedef Props user_type;
			struct node
			{
			public:
				rectangle bounds;
				user_type data;
				std::unique_ptr<node> l;
				std::unique_ptr<node> r;
				unsigned int count;

			public:
				bool leaf() const noexcept
				{
					return (l && r);
				}
				template <typename Random>
				unsigned int split(Random && rng, int min, int margin) // returns number of created leaves
				{
					int w = bounds.width();
					int h = bounds.height();

					// select axis to split
					bool horisontal = std::uniform_int_distribution<int>{ 0, 1 }(rng) == 0;
					if (w * 3 >= h * 4)
					{
						horisontal = true;
					}
					else if (h * 3 > w * 4)
					{
						horisontal = false;
					}

					int max = (horisontal ? w : h) - min - margin;
					if (max >= min)
					{
						l = std::make_unique<node>();
						r = std::make_unique<node>();
						int split_point = std::uniform_int_distribution<int>{ min, max }(rng);

						if (horisontal)
						{
							l->bounds = { bounds.start(), { split_point, h } };
							r->bounds = { bounds.start().moved(split_point + margin, 0), { w - split_point - margin, h } };
						}
						else
						{
							l->bounds = { bounds.start(), { w, split_point } };
							r->bounds = { bounds.start().moved(0, split_point + margin), { w, h - split_point - margin} };
						}

						count = l->split(rng, min, margin) + r->split(rng, min, margin);
					}
					else
					{
						count = 1;
					}

					return count;
				}
				template <typename Operator> void traverse(Operator && fn) const
				{
					if (l && r)
					{
						l->traverse(fn);
						r->traverse(fn);
					}
					else
					{
						fn(*this);
					}
				}
				template <typename Operator> void traverse(Operator && fn)
				{
					if (l && r)
					{
						l->traverse(fn);
						r->traverse(fn);
					}
					else
					{
						fn(*this);
					}
				}
				template <typename Operator> void traverse_bounds(Operator && fn)
				{
					if (l && r)
					{
						l->traverse_bounds(fn);
						r->traverse_bounds(fn);
					}
					else
					{
						fn(bounds);
					}
				}
				template <typename Operator> void traverse_bounds(Operator && fn) const
				{
					if (l && r)
					{
						l->traverse_bounds(fn);
						r->traverse_bounds(fn);
					}
					else
					{
						fn(bounds);
					}
				}
				template <typename Operator> void traverse_data(Operator && fn)
				{
					if (l && r)
					{
						l->traverse_data(fn);
						r->traverse_data(fn);
					}
					else
					{
						fn(data);
					}
				}
				template <typename Operator> void traverse_data(Operator && fn) const
				{
					if (l && r)
					{
						l->traverse_data(fn);
						r->traverse_data(fn);
					}
					else
					{
						fn(data);
					}
				}
			};

		public:
			template <typename Random>
			void split(Random && rng, int min, int margin)
			{
				m_margin = margin;
				m_root.bounds = m_bounds.deflated(margin);
				m_root.split(std::forward<Random>(rng), min, margin);
			}
			template <typename Operator> void enumerate(Operator && fn) const
			{
				m_root.traverse(std::forward<Operator>(fn));
			}
			template <typename Operator> void enumerate(Operator && fn)
			{
				m_root.traverse(std::forward<Operator>(fn));
			}
			template <typename Operator> void enumerate_bounds(Operator && fn) const
			{
				m_root.traverse_bounds(std::forward<Operator>(fn));
			}
			template <typename Operator> void enumerate_bounds(Operator && fn)
			{
				m_root.traverse_bounds(std::forward<Operator>(fn));
			}
			template <typename Operator> void enumerate_data(Operator && fn) const
			{
				m_root.traverse_data(std::forward<Operator>(fn));
			}
			template <typename Operator> void enumerate_data(Operator && fn)
			{
				m_root.traverse_data(std::forward<Operator>(fn));
			}
			unsigned int count() const noexcept
			{
				return m_root.count;
			}
			rectangle bounds() const noexcept
			{
				return m_bounds;
			}

			template <typename Random = std::mt19937>
			static bsp create(Random && rng, rectangle bounds, int min, int margin)
			{
				bsp paritition(bounds);
				paritition.split(std::forward<Random>(rng), min, margin);
				return paritition;
			}

		private:
			bsp(rectangle bounds)
				: m_bounds(bounds)
			{
			}

		private:
			node m_root;
			int m_margin;
			rectangle m_bounds;
		};
	}
}