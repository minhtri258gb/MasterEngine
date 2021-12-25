#define __MT_TEST_CPP__

#include "common.h"
#include "Test.h"
#include "graphic/Graphic.h"
#include "file/TestJson.h"
#include "file/TestIni.h"
#include "mod/PluginSystem.h"

using namespace mt::mod;
using namespace mt::test;

Test::Test()
{

}

Test::~Test()
{

}

void Test::run()
{
	// Graphic test; // pass
	// TestJson test; // faild hien loi nhung build duoc
	// TestIni test; // pass
	PluginSystem test;

	test.run();
}
