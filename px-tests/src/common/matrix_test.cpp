// name: matrix_test.cpp

#include "catch.hpp"

#include <px/common/matrix.hpp>

TEST_CASE("matrix", "[matrix]")
{
	const unsigned int W = 100;
	const unsigned int H = 100;
	int w = std::rand() % 20 + 20;
	int h = std::rand() % 20 + 20;

	int element = 100500;

	auto generator = [](size_t x, size_t y) -> int { return static_cast<int>(x) * 2 + static_cast<int>(y) - 1; };
	int g = generator(5, 5);

	// constructors
	px::matrix2<int, W, H> m1;
	px::matrix2<int, W, H> m2(element);
	px::matrix2<int, W, H> m3(generator);

	// attributes
	REQUIRE(m1.width() == W);
	REQUIRE(m1.height() == H);
	REQUIRE(m1.size() == W * H);

	// access
	REQUIRE((m3[{ 5, 5 }]) == g);
	REQUIRE(m1.select({ -1, -1 }, element + 1) == element + 1);
	REQUIRE_THROWS(m1.at({ 0, -1 }));
	REQUIRE_THROWS(m1.at({ -1, 0 }));
	REQUIRE_THROWS(m1.at({ 0, (int)H }));
	REQUIRE_THROWS(m1.at({ (int)W, 0 }));

	REQUIRE(m1.at(0, 0) == m1[px::point2(0, 0)]);

	px::matrix2<int> a(w, h);
	px::matrix2<int> c(w, h, element);
	px::matrix2<int> e(w, h, generator);

	REQUIRE_NOTHROW(px::matrix2<int> f1(0, h));
	REQUIRE_NOTHROW(px::matrix2<int> f2(w, 0));


	REQUIRE(a.width() == w);
	REQUIRE(a.height() == h);
	REQUIRE(a.size() == w * h);

	REQUIRE((e[{ 5, 5 }]) == g);
	REQUIRE(a.select({ -1, -1 }, element + 1) == element + 1);
	REQUIRE_THROWS(a.at({ 0, -1 }));
	REQUIRE_THROWS(a.at({ -1, 0 }));
	REQUIRE_THROWS(a.at({ 0, h }));
	REQUIRE_THROWS(a.at({ w, 0 }));

	// resize
	a.resize(3, 5);
	REQUIRE(a.width() == 3);
	REQUIRE(a.height() == 5);

	// swap
	c.swap(a);
	REQUIRE(c.width() == 3);
	REQUIRE(c.height() == 5);

	a.fill(element + 4);
	REQUIRE((a[{0, 0}]) == element + 4);
	a.fill([=](auto /*i*/, auto /* j */) -> int { return element + 5; });
	REQUIRE((a[{0, 0}]) == element + 5);
}