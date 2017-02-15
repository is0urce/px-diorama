// name: astar.h
// type: c++ header
// desc: pathfinding
// auth: is0urce

// Implementation of A* pathfinding

#pragma once

#include <px/common/point.hpp>

#include <memory>
#include <list>
#include <set>

namespace px
{
	class astar
	{
	private:
		struct coord;
		typedef std::shared_ptr<coord> coord_ptr;
		struct coord : public point2
		{
		public:
			double g_score;
			double f_score;
			coord_ptr came_from;
		public:
			coord(point2 p, double traversed, double heu, coord_ptr prev) : point2(p), g_score(traversed), f_score(traversed + heu), came_from(prev)
			{
			}
		};

		struct score_less
		{
			bool operator() (const coord_ptr a, const coord_ptr b) const
			{
				return a->f_score < b->f_score;
			}
		};

		struct lexicographical_less
		{
			template <typename _LPtr, typename _RPtr>
			bool operator() (const _LPtr lhs, const _RPtr rhs) const
			{
				return lhs->x() < rhs->x() || lhs->x() == rhs->x() && lhs->y() < rhs->y();
			}
			typedef point2* is_transparent; // enable homogenical comparation to raw pointers
		};

		static auto heuristic(const point2 &a, const point2 &b)
		{
			return a.distance(b);
		}

		static auto distance(const point2 &a, const point2 &b)
		{
			return a.king_distance(b);
		}

		static std::list<point2> neighbours(const point2 &c)
		{
			std::list<point2> list;
			list.emplace_back(c.x() + 1, c.y());
			list.emplace_back(c.x(), c.y() + 1);
			list.emplace_back(c.x() - 1, c.y());
			list.emplace_back(c.x(), c.y() - 1);
			// diagonals
			list.emplace_back(c.x() + 1, c.y() + 1);
			list.emplace_back(c.x() + 1, c.y() - 1);
			list.emplace_back(c.x() - 1, c.y() + 1);
			list.emplace_back(c.x() - 1, c.y() - 1);
			return list;
		}

		// reconstruct path from target to start
		static void construct_path(coord_ptr step, std::list<point2> &path)
		{
			while (step)
			{
				if (!step->came_from) break; // stop there and not include starting point as part of path, so first step is list::front()
				path.emplace_front(step->x(), step->y());
				step = step->came_from;
			}
		}

	public:
		// _TPr - traversable predicate bool(point)
		template <typename _TPr>
		static auto find(point2 start, point2 finish, _TPr traversable, unsigned int steps)
		{
			std::list<point2> path;
			std::multiset<coord_ptr, score_less> open;
			std::set<coord_ptr, lexicographical_less> closed;

			open.insert(std::make_shared<coord>(start, 0, heuristic(start, finish), nullptr));
			while (steps > 0 && !open.empty())
			{
				auto top = open.begin();
				coord_ptr current = *top;

				if (*current == finish) // terminal
				{
					construct_path(current, path);
					break;
				}

				open.erase(top);
				closed.insert(current);

				// expand
				for (point2 &neighbor : neighbours(*current))
				{
					if (!traversable(neighbor) && neighbor != finish) continue; // traversable callback
					if (closed.find(&neighbor) != closed.end()) continue; // already in closed list

					auto it = open.cbegin(), last = open.cend();
					while (it != last && **it != neighbor) ++it; // check is point in open list

					auto score = current->g_score + distance(*current, neighbor);
					if (it == last) // not in open list
					{
						open.insert(std::make_shared<coord>(neighbor, score, heuristic(neighbor, finish), current));
					}
					else if (score < (*it)->g_score) // better option
					{
						open.insert(open.erase(it), std::make_shared<coord>(neighbor, score, heuristic(neighbor, finish), current));
					}
				}
				--steps;
			}
			return path;
		}
	};
}