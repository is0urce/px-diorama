// name: intrusive_test.cpp

#include "catch.hpp"

#include <px/common/intrusive_ptr.hpp>

namespace intrusive_test {

	class foo : public px::intrusive
	{
	};
	class bar : public px::intrusive
	{
	};

	static int t_counter = 0;

	class obj : public px::intrusive
	{
	public:
		obj(obj&)
		{
			++t_counter;
		}
		obj()
		{
			++t_counter;
		}
		obj(std::string message)
		{
			++t_counter;
		}
		~obj()
		{
			--t_counter;
		}
	};

	class foo_deleter
	{
	public:
		void operator()(foo* x)
		{
			delete x;
		}
	};
	class obj_deleter
	{
	public:
		void operator()(obj* x)
		{
			delete x;
		}
	};

	TEST_CASE("intrusive_ptr", "[intrusive_ptr]")
	{
		SECTION("starts with nullptr") {
			px::intrusive_ptr<foo, foo_deleter> f;
			px::intrusive_ptr<bar> b;

			REQUIRE(b == false);
			REQUIRE(f == false);
			REQUIRE(b.get() == nullptr);
			REQUIRE(f.get() == nullptr);
		}

		SECTION("starts with pointer") {
			px::intrusive_ptr<foo, foo_deleter> f(new foo);
			px::intrusive_ptr<bar> b(new bar);

			REQUIRE(b == true);
			REQUIRE(f == true);
			REQUIRE(b.get() != nullptr);
			REQUIRE(f.get() != nullptr);
		}

		REQUIRE(t_counter == 0);
		SECTION("release") {
			px::intrusive_ptr<obj> f1(new obj);
			px::intrusive_ptr<obj, obj_deleter> f2(new obj);
			REQUIRE(t_counter == 2);
			REQUIRE(f1->intrusive_counter() == 1);
		}

		SECTION("clone") {
			px::intrusive_ptr<obj> f1(new obj);
			px::intrusive_ptr<obj> f2(f1);
			px::intrusive_ptr<obj> f3 = f2;

			REQUIRE(t_counter == 1);
			REQUIRE(f1->intrusive_counter() == 3);
		}

		SECTION("move") {
			px::intrusive_ptr<obj> f1;
			
			f1 = px::intrusive_ptr<obj>(new obj);

			REQUIRE(t_counter == 1);
			REQUIRE(f1->intrusive_counter() == 1);

			// and another one

			f1 = px::intrusive_ptr<obj>(new obj);

			REQUIRE(t_counter == 1);
			REQUIRE(f1->intrusive_counter() == 1);
		}

		REQUIRE(t_counter == 0);
	}

}