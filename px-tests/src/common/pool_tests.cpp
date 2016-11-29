// name: pool_tests.cpp

#include "catch.hpp"

#include <px/common/pool.hpp>

#include <algorithm>
#include <list>
#include <random>
#include <iostream>
#include <string>
#include <vector>

static int g_counter = 0;

class obj
{
public:
	obj(obj&)
	{
		//std::cout << "+";
		++g_counter;
	}
	obj()
	{
		//std::cout << "+";
		++g_counter;
	}
	obj(std::string message)
	{
		//std::cout << "\n" << message;
		++g_counter;
	}
	~obj()
	{
		//std::cout << "-";
		--g_counter;
	}
};

TEST_CASE("pools are open", "[pool]")
{
	g_counter = 0;
	auto is_sequental = [](auto& pool) {
		const char* last = nullptr;
		bool flag = true;
		pool.enumerate([&last, &flag](auto &e) {
			const char* ptr = reinterpret_cast<const char*>(&e);
			if (ptr <= last) flag = false; // next in memory
			last = ptr;
		});
		return flag;
	};

	auto count = [](auto& pool) {
		size_t counter = 0;
		pool.enumerate([&counter](auto&) { ++counter; });
		return counter;
	};

	const size_t maximum = 100;
	typedef obj element;
	typedef px::pool<element, maximum> pool_type;

	pool_type p; // = a; // an error

	REQUIRE(p.size() == 0);
	REQUIRE(count(p) == 0);
	REQUIRE(p.empty() == true);
	REQUIRE(p.size() == g_counter);
	REQUIRE(p.max_size() == maximum);

	SECTION("querry and release")
	{
		element* first;

		first = p.request("MASSAGE");
		REQUIRE(p.size() == g_counter);
		REQUIRE(first != nullptr);
		REQUIRE(p.size() == 1);
		REQUIRE(count(p) == 1);
		REQUIRE(p.empty() == false);
		REQUIRE(p.contains(first) == true);
		REQUIRE(p.contains(nullptr) == false);

		p.release(first);
		REQUIRE(p.size() == 0);
		REQUIRE(count(p) == 0);
		REQUIRE(p.empty() == true);
		REQUIRE(p.size() == g_counter);
	}

	SECTION("limitations and clearance")
	{
		std::list<element*> list;
		for (size_t i = 0; i < maximum; ++i)
		{
			list.push_back(p.request());
		}
		REQUIRE(p.size() == maximum);
		REQUIRE(count(p) == maximum);
		REQUIRE(is_sequental(p) == true);
		REQUIRE(p.full() == true);
		REQUIRE(p.size() == g_counter);

		// everything else is nullptr
		REQUIRE(p.request() == nullptr);
		REQUIRE(p.size() == maximum);
		REQUIRE(count(p) == maximum);
		REQUIRE(p.full() == true);
		REQUIRE(p.size() == g_counter);

		for (auto eptr : list)
		{
			p.release(eptr);
		}
		list.clear();
		REQUIRE(p.size() == 0);
		REQUIRE(count(p) == 0);
		REQUIRE(p.empty() == true);
		REQUIRE(p.size() == g_counter);

		// clear
		p.clear();
		REQUIRE(p.size() == 0);
		REQUIRE(count(p) == 0);
		REQUIRE(p.empty() == true);
		REQUIRE(p.size() == g_counter);
	}

	SECTION("limitations and clearance extended - smart pointers")
	{
		// unique scope
		{
			pool_type::unique_ptr u1 = p.make_unique();
			auto u2 = p.make_unique("with arguments");
			REQUIRE(p.size() == 2);
			REQUIRE(count(p) == 2);
			REQUIRE(p.size() == g_counter);
		}
		REQUIRE(p.size() == 0);
		REQUIRE(count(p) == 0);
		REQUIRE(p.empty() == true);
		REQUIRE(p.size() == g_counter);

		// shared
		std::list<px::shared_ptr<element>> list;
		for (int i = 0; i < maximum; ++i)
		{
			list.push_back(p.make_shared("with arguments"));
		}
		REQUIRE(p.size() == maximum);
		REQUIRE(count(p) == maximum);

		list.clear();
		REQUIRE(p.size() == 0);
		REQUIRE(count(p) == 0);
		REQUIRE(p.empty() == true);
		REQUIRE(p.size() == g_counter);
	}

	SECTION("RNG MADNESS SHOW")
	{
		std::mt19937 rng;
		std::vector<element*> arr;
		arr.reserve(maximum);

		auto dump = [](auto &pool) {
			std::vector<element*> vec;
			pool.enumerate([&vec](auto &e) { vec.push_back(&e); });
			return vec;
		};

		for (int j = 0; j < 50; ++j)
		{
			arr.push_back(p.request());
		}
		for (int k = 0; k < 10; ++k)
		{
			for (int j = 0; j < 25; ++j)
			{
				arr.push_back(p.request());
			}
			for (int j = 0; j < 25; ++j)
			{
				size_t index = std::uniform_int_distribution<size_t>{ 0, arr.size() - 1 }(rng);
				p.release(arr[index]);
				arr.erase(std::begin(arr) + index);
			}
		}
		std::sort(std::begin(arr), std::end(arr));
		auto current = dump(p);

		REQUIRE(count(p) == p.size());
		REQUIRE(is_sequental(p) == true);
		REQUIRE(count(p) == arr.size());
		REQUIRE(std::equal(std::begin(arr), std::begin(arr) + arr.size(), std::begin(current)) == true);

		REQUIRE(p.size() == g_counter);
	}
}