// name: qtree_test.cpp

#include "catch.hpp"

#include <px/common/qtree.hpp>

TEST_CASE("qtree", "[qtree]")
{
	const unsigned int range = 64;

	px::qtree<int> tree(range);

	REQUIRE_FALSE(tree.contains(0, 64));
	REQUIRE(tree.contains(-64, -64));
	
	tree.add(5, 5, 6);
	tree.add(100, 100, 8);
	REQUIRE_THROWS(tree.remove(3, 3, 2));
	tree.remove(5, 5, 6);
	REQUIRE_THROWS(tree.remove(5, 5, 6));
	tree.remove(100, 100, 8);

	tree.add(3, 3, 6);
	tree.move(3, 3, 6, 8, 8);
	REQUIRE_THROWS(tree.remove(3, 3, 6));
	tree.remove(8, 8, 6);

	std::vector<int> found;
	auto fn = [&](int /* x */, int /* y */, int e) { found.push_back(e); return true; };

	tree.find(0, 0, 100, fn);
	tree.find(0, 0, fn);
}