// name: main.cpp
// type: c++
// auth: is0urce
// desc: entry point generated there

#define CATCH_CONFIG_RUNNER // providing own main
#include "test.hpp"

//int _tmain(int argc, _TCHAR* argv[])
int main(int argc, char const* const argv[])
{
	Catch::Session().run(argc, argv);
	return 0;
}