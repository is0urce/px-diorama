// name: coordinate.hpp
// type: c++ header
// desc: base template class for points and vectors

#pragma once

#include <px/common/point.hpp>

#include <algorithm>
#include <array>
#include <vector>
#include <stdexcept>

namespace px {

	template <typename Element, size_t...> class matrix2;

	// matrix with fixed sizes

	template <typename Element, size_t W, size_t H>
	class matrix2<Element, W, H>
	{
	public:
		typedef Element element_type;

	public:
		constexpr size_t width() const
		{
			return W;
		}
		constexpr size_t height() const
		{
			return H;
		}
		constexpr size_t size() const
		{
			return W * H;
		}
		constexpr point2 range() const
		{
			return point2(static_cast<int>(W), static_cast<int>(H));
		}

		void swap(matrix2 & that)
		{
			std::swap(m_data, that.m_data);
		}
		void copy(matrix2 const& that)
		{
			std::copy(that.m_data[0], that.m_data[W * H], m_data[0]);
		}

		bool contains(coordinate<int, 2> const& position) const
		{
			return contains(position.get<0>(), position.get<1>());
		}
		bool contains(point2 const& position) const
		{
			return contains(position.x(), position.y());
		}
		constexpr bool contains(size_t x, size_t y) const noexcept
		{
			return x >= 0 && x < W && y >= 0 && y < H;
		}

		void fill(element_type const& e)
		{
			m_data.fill(e);
		}
		template <typename Generator>
		void fill(Generator op)
		{
			size_t index = 0;
			for (size_t j = 0; j != H; ++j) {
				for (size_t i = 0; i != W; ++i) {
					m_data[index] = op(i, j);
					++index;
				}
			}
		}

		template <typename Operator>
		void enumerate(Operator && op)
		{
			size_t index = 0;
			for (size_t j = 0; j != H; ++j) {
				for (size_t i = 0; i != W; ++i) {
					op(point2(static_cast<point2::component>(i), static_cast<point2::component>(j)), m_data[index]);
					++index;
				}
			}
		}
		template <typename Operator>
		void enumerate(Operator && op) const
		{
			size_t index = 0;
			for (size_t j = 0; j != H; ++j) {
				for (size_t i = 0; i != W; ++i) {
					op(point2(static_cast<point2::component>(i), static_cast<point2::component>(j)), m_data[index]);
					++index;
				}
			}
		}

		// querry functions: operator[] not throws, at() throws, select returns default (outer) if out of range
		// specialized point2 acessors for easy querry with bracket-initialized points
		template <typename Component>
		element_type const& operator[](coordinate<Component, 2> key) const
		{
			return m_data[key.get<1>() * W + key.get<0>()];
		}
		template <typename Component>
		element_type & operator[](coordinate<Component, 2> key)
		{
			return m_data[key.get<1>() * W + key.get<0>()];
		}
		element_type const& operator[](point2 key) const
		{
			return m_data[key.y() * W + key.x()];
		}
		element_type & operator[](point2 key)
		{
			return m_data[key.y() * W + key.x()];
		}

		template<typename Component>
		element_type const& at(coordinate<Component, 2> key) const
		{
			if (!contains(key)) throw std::runtime_error("px::matrix<e,w,h>::at() - out of range");
			return operator[](key);
		}
		template<typename Component>
		element_type & at(coordinate<Component, 2> key)
		{
			if (!contains(key)) throw std::runtime_error("px::matrix<e,w,h>::at() - out of range");
			return operator[](key);
		}
		element_type const& at(point2 key) const
		{
			if (!contains(key)) throw std::runtime_error("px::matrix<e,w,h>::at(point) - position out of range");
			return operator[](key);
		}
		element_type & at(point2 key)
		{
			if (!contains(key)) throw std::runtime_error("px::matrix<e,w,h>::at(point) - out of range");
			return operator[](key);
		}
		element_type const& at(size_t x, size_t y) const
		{
			if (!contains(x, y)) throw std::runtime_error("px::matrix<e,w,h>::at() - out of range");
			return m_data[y * W + x];
		}
		element_type & at(size_t x, size_t y)
		{
			if (!contains(x, y)) throw std::runtime_error("px::matrix<e,w,h>::at() - out of range");
			return m_data[y * W + x];
		}

		template<typename Component>
		element_type const& select(coordinate<Component, 2> key, element_type const& outer) const
		{
			return contains(key) ? operator[](key) : outer;
		}
		template<typename Component>
		element_type & select(coordinate<Component, 2> key, element_type & outer)
		{
			return contains(key) ? operator[](key) : outer;
		}
		element_type const& select(point2 key, element_type const& outer) const
		{
			return contains(key) ? operator[](key) : outer;
		}
		element_type & select(point2 key, element_type & outer)
		{
			return contains(key) ? operator[](key) : outer;
		}
		element_type const& select(unsigned int x, unsigned int y, element_type const& outer) const
		{
			return contains(x, y) ? m_data[y * W + x];
		}
		element_type & select(unsigned int x, unsigned int y, element_type & outer)
		{
			return contains(x, y) ? m_data[y * W + x];
		}

	public:
		matrix2()
		{
		}
		matrix2(element_type const& initial)
		{
			fill(initial);
		}
		template <typename Generator>
		matrix2(Generator op)
		{
			fill(std::forward<Generator>(op));
		}
		matrix2(matrix2 const&) = delete;
		matrix2 & operator=(matrix2 const&) = delete;
		matrix2(matrix2 &&) = default;
		matrix2 & operator=(matrix2 &&) = default;

	private:
		std::array<element_type, W * H> m_data;
	};

	// matrix with varying size

	template <typename Element>
	class matrix2<Element>
	{
	public:
		typedef Element element;

	public:
		matrix2()
			: m_width(0), m_height(0)
		{
		}
		matrix2(matrix2 && that)
			: matrix2()
		{
			swap(that);
		}
		matrix2(size_t w, size_t h)
			: m_width(w), m_height(h)
		{
			m_data.resize(w * h);
		}
		matrix2(size_t w, size_t h, element const& initial)
			: m_width(w), m_height(h)
		{
			m_data.assign(w * h, initial);
		}
		matrix2(point2 const& range)
			: matrix2(range.x(), range.y())
		{
		}
		template <typename Generator, typename = typename std::enable_if<!std::is_same<Generator, element>::value>::type>
		matrix2(size_t w, size_t h, Generator op)
			: matrix2(w, h)
		{
			generate(std::forward<Generator>(op));
		}
		matrix2(matrix2 const&) = delete;
		matrix2& operator=(matrix2 const&) = delete;

		void resize(size_t w, size_t h)
		{
			m_width = w;
			m_height = h;
			m_data.resize(w * h);
		}
		void resize(point2 const& range)
		{
			resize(range.x(), range.y());
		}

		void swap(matrix2 &that)
		{
			std::swap(m_height, that.m_height);
			std::swap(m_width, that.m_width);
			std::swap(m_data, that.m_data);
		}

		size_t width() const noexcept
		{
			return m_width;
		}
		size_t height() const noexcept
		{
			return m_height;
		}
		point2 range() const
		{
			return point2(static_cast<point2::component>(m_width), static_cast<point2::component>(m_height));
		}
		size_t size() const noexcept
		{
			return m_width * m_height;
		}
		bool empty() const noexcept
		{
			return m_width == 0 || m_height == 0;
		}
		bool contains(coordinate<int, 2> const& position) const
		{
			return contains(position[0], position[1]);
		}
		bool contains(point2 const& position) const
		{
			return contains(position.x(), position.y());
		}
		bool contains(unsigned int x, unsigned int y) const
		{
			return x >= 0 && x < m_width && y >= 0 && y < m_height;
		}

		void fill(element const& e)
		{
			size_t len = size();
			for (size_t i = 0; i != len; ++i) {
				m_data[i] = e;
			}
		}
		template <typename Generator>
		void generate(Generator && op)
		{
			size_t index = 0;
			for (size_t j = 0; j != m_height; ++j) {
				for (size_t i = 0; i != m_width; ++i) {
					m_data[index] = op(i, j);
					++index;
				}
			}
		}
		template <typename Operator>
		void enumerate(Operator && op)
		{
			size_t index = 0;
			for (size_t j = 0; j != m_height; ++j) {
				for (size_t i = 0; i != m_width; ++i) {
					op(point2(static_cast<point2::component>(i), static_cast<point2::component>(j)), m_data[index]);
					++index;
				}
			}
		}
		template <typename Operator>
		void enumerate(Operator && op) const
		{
			size_t index = 0;
			for (size_t j = 0; j != m_height; ++j) {
				for (size_t i = 0; i != m_width; ++i) {
					op(point2(static_cast<point2::component>(i), static_cast<point2::component>(j)), m_data[index]);
					++index;
				}
			}
		}

		// querry functions: operator[] not throws, at() throws, select returns default (outer) if out of range
		// specialized point2 acessors for easy querry with bracket-initialized points

		template<typename Component>
		element const& operator[](coordinate<Component, 2> const& key) const
		{
			return m_data[key.get<1>() * m_width + key.get<0>()];
		}
		template<typename Component>
		element & operator[](coordinate<Component, 2> const& key)
		{
			return m_data[key.get<1>() * m_width + key.get<0>()];
		}
		element const& operator[](point2 const& key) const
		{
			return m_data[key.y() * m_width + key.x()];
		}
		element & operator[](point2 const& key)
		{
			return m_data[key.y() * m_width + key.x()];
		}

		template<typename Component>
		element const& at(coordinate<Component, 2> const& key) const
		{
			if (!contains(key)) throw std::runtime_error("px::matrix<e>::at() - out of range");
			return (*this)[key];
		}
		template<typename Component>
		element & at(coordinate<Component, 2> const& key)
		{
			if (!contains(key)) throw std::runtime_error("px::matrix<e>::at() - out of range");
			return (*this)[key];
		}
		element const& at(point2 const& key) const
		{
			if (!contains(key)) throw std::runtime_error("px::matrix<e>::at() - out of range");
			return (*this)[key];
		}
		element & at(point2 const& key)
		{
			if (!contains(key)) throw std::runtime_error("px::matrix<e>::at() - out of range");
			return (*this)[key];
		}
		element const& at(unsigned int x, unsigned int y) const
		{
			if (!contains(x, y)) throw std::runtime_error("px::matrix<e>::at() - out of range");
			return m_data[y * m_width + x];
		}
		element & at(unsigned int x, unsigned int y)
		{
			if (!contains(x, y)) throw std::runtime_error("px::matrix<e>::at() - out of range");
			return m_data[y * m_width + x];
		}

		template <typename Component>
		element const& select(coordinate<Component, 2> const& key, element const& outer) const
		{
			return contains(key) ? (*this)[key] : outer;
		}
		template <typename Component>
		element & select(coordinate<Component, 2> const& key, element & outer)
		{
			return contains(key) ? (*this)[key] : outer;
		}
		element const& select(point2 const& key, element const& outer) const
		{
			return contains(key) ? (*this)[key] : outer;
		}
		element & select(point2 const& key, element & outer)
		{
			return contains(key) ? (*this)[key] : outer;
		}
		element const& select(unsigned int x, unsigned int y, element const& outer) const
		{
			return contains(x, y) ? m_data[y * m_width + x];
		}
		element & select(unsigned int x, unsigned int y, element & outer)
		{
			return contains(x, y) ? m_data[y * m_width + x];
		}

	private:
		std::vector<element> m_data;
		size_t m_width;
		size_t m_height;
	};
}