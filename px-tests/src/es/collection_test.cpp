// name: collection_test.cpp

#include "catch.hpp"

#include <px/common/shared_ptr.hpp>
#include <px/es/component_collection.hpp>

namespace component_collection_test {

	class head : public px::es::component
	{

	};

	class leg : public px::es::component
	{

	};

	TEST_CASE("component collection test", "[component collection test]")
	{
		px::es::component_collection<px::shared_ptr<px::es::component>> cc;

		auto h = px::make_shared<head>();

		cc.add(px::make_shared<leg>());
		cc.add(h);

		SECTION("query")
		{
			REQUIRE(cc.component<head>() == h);
		}

		cc.enable();
		REQUIRE(h->active() == true);

		SECTION("deactivate")
		{
			cc.disable();
			REQUIRE(h->active() == false);
		}
		SECTION("remove by value")
		{
			cc.remove(h);
			cc.disable();
			REQUIRE(h->active() == true);
		}
		SECTION("remove by type")
		{
			cc.remove<head>();
			cc.disable();
			REQUIRE(h->active() == true);
		}
		SECTION("remove all")
		{
			cc.clear();
			REQUIRE(h->active() == true);
		}
	}
}