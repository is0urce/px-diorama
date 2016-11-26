// name: link_test.cpp

#include "catch.hpp"

#include <px/es/link.hpp>

class pos;
class body;

class hand {};
class leg {};
class soul;
class pos {};

class body
	: public px::link_dispatcher<body>
	, public px::link<pos>
	, public px::link<hand>
	, public px::link<leg>
{
};

class soul
	: public px::link_dispatcher<soul>
	, public px::link<pos>
{

};

TEST_CASE("link", "[link]")
{
	pos p;
	body b;
	soul s;

	hand h1;
	leg l1;

	SECTION("assign and retrieve")
	{
		REQUIRE(b.linked<hand>() == nullptr);

		b.assign(&h1);
		b.assign(&l1);
		b.assign(&p);

		hand* h = b.linked<hand>();
		leg* l = b;

		REQUIRE(&h1 == h);
		REQUIRE(&l1 == l);

		b.retract<hand>();
		REQUIRE(b.linked<hand>() == nullptr);
	}
}