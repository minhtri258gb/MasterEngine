#define __TEST_CPP__

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
	graphic::Graphic test_graphic;
	test_graphic.run();
}
