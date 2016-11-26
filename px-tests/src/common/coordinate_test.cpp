// name: matrix_test.cpp

#include "catch.hpp"

#include <px/common/point.hpp>
#include <px/common/vector.hpp>

TEST_CASE("coordinate", "[point distance]")
{
	px::point2 p0(6, 8);
	px::point2 p1(3, 4);
	px::point2 zero(0, 0);
	px::point2 p3(-3, 4);

	// attributes
	REQUIRE(p0.distance(p1) == p1.distance(p0));
	REQUIRE(zero.king_distance(p1) == 4);
	REQUIRE(zero.king_distance(p3) == 4);
	REQUIRE(zero.block_distance(p1) == 7);

	constexpr px::coordinate<int, 2> c(0, 0);
	constexpr px::point2 cp(1, 0);
	constexpr px::vector2 cv(0.0, 0.0);

	constexpr px::point2 p_copy_p(cp);
	constexpr px::vector2 v_copy_p(cp);
	constexpr px::vector2 v_copy_v(cv);

	REQUIRE(p_copy_p.x() == 1);

	auto t = c;
	t.mirror(0);
	t.mirror<0>();
}