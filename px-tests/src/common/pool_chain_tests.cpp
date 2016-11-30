// name: pool_chain_tests.cpp

#include "catch.hpp"

#include <px/common/pool_chain.hpp>

#include <algorithm>
#include <list>
#include <random>
#include <vector>

TEST_CASE("pool_chain", "[pool_chain]")
{
	auto count = [](auto const& pool) {
		size_t counter = 0;
		pool.enumerate([&counter](auto const&) { ++counter; });
		return counter;
	};

	const size_t maximum = 10;
	typedef int element;
	typedef px::pool_chain<element, maximum> pool_type;

	pool_type p;
	REQUIRE(p.size() == 0);
	REQUIRE(count(p) == 0);
	REQUIRE(p.empty() == true);
	REQUIRE(p.contains(nullptr) == false);

	SECTION("beyound limits")
	{
		element* first = p.request();
		REQUIRE(first != nullptr);
		REQUIRE(p.size() == 1);
		REQUIRE(count(p) == 1);
		REQUIRE(p.empty() == false);
		REQUIRE(p.contains(first) == true);

		p.release(first);
		REQUIRE(p.size() == 0);
		REQUIRE(count(p) == 0);
		REQUIRE(p.empty() == true);
	}

	SECTION("clearance")
	{
		std::list<element*> list;

		for (size_t i = 0; i < maximum; ++i)
		{
			list.push_back(p.request());
		}
		REQUIRE(p.size() == maximum);
		REQUIRE(count(p) == maximum);

		// can query above maximum
		list.push_back(p.request(3));
		REQUIRE(list.back() != nullptr);
		REQUIRE(p.size() == maximum + 1);
		REQUIRE(count(p) == maximum + 1);

		for (auto eptr : list)
		{
			p.release(eptr);
		}
		list.clear();
		REQUIRE(p.size() == 0);
		REQUIRE(count(p) == 0);
		REQUIRE(p.empty() == true);

		for (size_t i = 0; i < maximum; ++i)
		{
			list.push_back(p.request());
		}
		REQUIRE(p.size() == maximum);
		REQUIRE(count(p) == maximum);

		// clear
		p.clear();
		REQUIRE(p.size() == 0);
		REQUIRE(count(p) == 0);
		REQUIRE(p.empty() == true);
	}

	SECTION("clearance extended - smart pointers")
	{
		// unique scope
		{
			pool_type::unique_ptr u1 = p.make_unique();
			auto u2 = p.make_unique();
			REQUIRE(p.size() == 2);
			REQUIRE(count(p) == 2);
		}
		REQUIRE(p.size() == 0);
		REQUIRE(count(p) == 0);
		REQUIRE(p.empty() == true);

		// shared
		std::list<px::shared_ptr<element>> list;
		for (int i = 0; i < maximum; ++i)
		{
			list.push_back(p.make_shared());
		}
		REQUIRE(p.size() == maximum);
		REQUIRE(count(p) == maximum);

		list.push_back(p.make_shared());
		REQUIRE(p.size() == maximum + 1);
		REQUIRE(count(p) == maximum + 1);


		list.clear();
		REQUIRE(p.size() == 0);
		REQUIRE(count(p) == 0);
		REQUIRE(p.empty() == true);
	}


	////// RNG MADNESS SHOW //////
	std::mt19937 rng;
	p.clear();
	std::vector<element*> arr;
	arr.reserve(maximum * 5);

	auto dump = [](auto &pool) {
		std::vector<element*> vec;
		pool.enumerate([&vec](auto &e) { vec.push_back(&e); });
		return vec;
	};

	for (int j = 0; j < maximum * 3; ++j)
	{
		arr.push_back(p.request());
	}
	for (int k = 0; k < 10; ++k)
	{
		for (int j = 0; j < maximum; ++j)
		{
			arr.push_back(p.request());
		}
		for (int j = 0; j < maximum; ++j)
		{
			size_t index = std::uniform_int_distribution<size_t>{ 0, arr.size() - 1 }(rng);
			p.release(arr[index]);
			arr.erase(std::begin(arr) + index);
		}
	}
	auto current = dump(p);
	std::sort(std::begin(arr), std::end(arr));
	std::sort(std::begin(current), std::end(current));

	REQUIRE(count(p) == p.size());
	REQUIRE(count(p) == arr.size());
	REQUIRE(std::equal(arr.begin(), arr.begin() + arr.size(), current.begin()) == true);

	p.optimize();
}