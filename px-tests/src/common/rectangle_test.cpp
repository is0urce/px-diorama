// name: rectangle_test.cpp

#include "catch.hpp"

#include <px/common/rectangle.hpp>

TEST_CASE("rectangle", "[rectangle]")
{
	px::rectangle rect({ 2, 2 }, { 10, 10 });
	px::rectangle orig = rect;

	rect.inflate(1);

	REQUIRE(rect.start() == px::point2(1, 1));
	REQUIRE(rect.corner() == px::point2(13, 13));
	REQUIRE(rect.range() == px::point2(12, 12));

	// inflate-deflate
	REQUIRE(orig.inflated(1) == rect);
	REQUIRE(orig.inflated(10).deflated(10) == orig);
	rect.deflate(1);
	REQUIRE(rect == orig);

	// intersection test
	px::rectangle a({ 0, 0 }, { 10, 10 });
	px::rectangle b({ 9, 9 }, { 1, 1 });
	px::rectangle c({ 10, 10 }, { 0, 0 });
	REQUIRE(a.intersection(b) == b);
	REQUIRE(a.intersection(c).empty());

	// border test
	px::rectangle x({ 5, 5 }, { 5, 5 });
	REQUIRE(!x.is_border({ 4, 4 }));
	REQUIRE(x.is_border({ 5, 5 }));
	REQUIRE(!x.is_border({ 6, 6 }));
	REQUIRE(!x.is_border({ 8, 8 }));
	REQUIRE(x.is_border({ 9, 9 }));
	REQUIRE(!x.is_border({ 10, 10 }));

	// corner test
	REQUIRE(x.is_corner({ 5, 5 }));
	REQUIRE(!x.is_corner({ 5, 6 }));
	REQUIRE(x.is_corner({ 9, 9 }));

	// contains test
	REQUIRE(a.contains({ 0,0 }));
	REQUIRE(!a.contains({ 10, 10 }));
	REQUIRE(a.contains({ 9,9 }));
	REQUIRE(!a.contains({ -1, 0 }));
}