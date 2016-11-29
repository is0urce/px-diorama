// name: intrusive_test.cpp

#include "catch.hpp"

#include <px/common/shared_ptr.hpp>

namespace shared_test {

	class foo
	{
	};
	class bar
	{
	};

	struct deleter
	{
		void operator()(foo* ptr)
		{
			delete ptr;
		}
	};

	static int t_counter = 0;

	class obj
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

	TEST_CASE("shared_ptr", "[shared_ptr]")
	{
		px::ctrl_block<foo, deleter> fb;
		px::ctrl_block<bar> bb;

		SECTION("starts with nullptr") {
			px::shared_ptr<foo> f;
			px::shared_ptr<bar> b;

			REQUIRE(b == false);
			REQUIRE(f == false);
			REQUIRE(b.get() == nullptr);
			REQUIRE(f.get() == nullptr);
		}

		SECTION("starts with pointer") {
			px::shared_ptr<foo> f(new foo, &fb);
			px::shared_ptr<bar> b(new bar, &bb);

			px::shared_ptr<obj> xx(new obj);

			REQUIRE(b == true);
			REQUIRE(f == true);
			REQUIRE(b.get() != nullptr);
			REQUIRE(f.get() != nullptr);
		}

		REQUIRE(t_counter == 0);
		SECTION("release") {

			px::ctrl_block<obj> block1;
			px::ctrl_block<obj> block2;

			px::shared_ptr<obj> f1(new obj, &block1);
			px::shared_ptr<obj> f2(new obj, &block2);
			REQUIRE(t_counter == 2);
			REQUIRE(block1.counter() == 1);
			REQUIRE(block2.counter() == 1);
		}

		SECTION("clone") {

			px::ctrl_block<obj> block1;

			px::shared_ptr<obj> f1(new obj, &block1);
			px::shared_ptr<obj> f2(f1);
			px::shared_ptr<obj> f3 = f2;

			REQUIRE(t_counter == 1);
			REQUIRE(block1.counter() == 3);
		}

		SECTION("move") {
			px::ctrl_block<obj> block1;
			px::ctrl_block<obj> block2;

			px::shared_ptr<obj> f1;

			f1 = px::shared_ptr<obj>(new obj, &block1);

			REQUIRE(t_counter == 1);
			REQUIRE(block1.counter() == 1);

			// and another one

			f1 = px::shared_ptr<obj>(new obj, &block2);

			REQUIRE(t_counter == 1);
			REQUIRE(block1.counter() == 0);
			REQUIRE(block2.counter() == 1);
		}

		SECTION("move") {

			px::shared_ptr<obj> x = px::make_shared<obj>();
			REQUIRE(t_counter == 1);
		}

		REQUIRE(t_counter == 0);
	}

}