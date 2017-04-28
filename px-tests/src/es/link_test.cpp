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
	: public px::es::link_dispatcher<body>
	, public px::es::link<pos>
	, public px::es::link<hand>
	, public px::es::link<leg>
{
};

class soul
	: public px::es::link_dispatcher<soul>
	, public px::es::link<pos>
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

		b.connect(&h1);
		b.connect(&l1);
		b.connect(&p);

		hand* h = b.linked<hand>();
		leg* l = static_cast<leg*>(b);

		REQUIRE(&h1 == h);
		REQUIRE(&l1 == l);

		b.disconnect<hand>();
		REQUIRE(b.linked<hand>() == nullptr);
	}
}