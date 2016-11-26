// name: qtree_test.cpp

#include "catch.hpp"

#include <px/common/qtree.hpp>

TEST_CASE("qtree", "[qtree]")
{
	const unsigned int range = 64;


	px::qtree<int> tree(range);
	std::vector<int> found;
	auto fn = [&](int /* x */, int /* y */, auto e) { found.push_back(e); };

	SECTION("range") {
		REQUIRE_FALSE(tree.covers(0, 64));
		REQUIRE(tree.covers(-64, -64));
	}
	
	SECTION("add and remove") {
		tree.add(5, 5, 6);
		tree.add(5, 5, 7); // same position
		tree.add(100, 100, 8); // out of range

		tree.remove(5, 5, 6);
		tree.remove(5, 5, 7);
		tree.remove(100, 100, 8);

		REQUIRE_THROWS(tree.remove(5, 5, 6)); // is no more
		REQUIRE_THROWS(tree.remove(3, 3, 2)); // not exists
	}

	SECTION("move") {
		tree.add(3, 3, 6);
		tree.move(3, 3, 6, 8, 8);
		REQUIRE_THROWS(tree.remove(3, 3, 6));
		tree.remove(8, 8, 6);
	}

	SECTION("enumerate") {
		tree.find(0, 0, 100, fn);
		tree.find(0, 0, fn);

		tree.add(6, 6, 23);
		tree.add(6, 6, 24);
		tree.add(6, 6, 25);
		tree.find(6, 6, fn);
		REQUIRE(found.size() == 3);
	}
}