#define __MT_TEST_CPP__

#include "common.h"
#include "Test.h"
#include "graphic/Graphic.h"

using namespace mt::test;

Test::Test()
{

}

Test::~Test()
{

}

void Test::run()
{
	// Test 1
	// graphic::Graphic test_graphic;
	// test_graphic.run();

	// Test 2
	int flag = 0;
	enum
	{
		flag_a = 1 << 0,
		flag_b = 1 << 1,
		flag_c = 1 << 2,
		flag_d = 1 << 3
	};

	std::cout << "flag_a = " << flag_a << std::endl;
	std::cout << "flag_b = " << flag_b << std::endl;
	std::cout << "flag_c = " << flag_c << std::endl;
	std::cout << "flag_d = " << flag_d << std::endl;

	flag = flag_b | flag_d;
	std::cout << "flag_b | flag_d = " << flag << std::endl;


	int bit = (flag >> flag_a) & 1;
	std::cout << "(flag >> flag_a) & 1 = " << bit << std::endl;

	bit = (flag >> flag_b) & 1;
	std::cout << "(flag >> flag_b) & 1 = " << bit << std::endl;

	bit = (flag >> flag_c) & 1;
	std::cout << "(flag >> flag_c) & 1 = " << bit << std::endl;

	bit = (flag >> flag_d) & 1;
	std::cout << "(flag >> flag_d) & 1 = " << bit << std::endl;

}
