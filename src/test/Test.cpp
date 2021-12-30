#define __MT_TEST_CPP__

#include "common.h"
#include "Test.h"
#include "file/TestJson.h"
#include "audio/Audio.h"
#include "technique/TestTechnique.h"

using namespace mt::test;

void Test::run()
{
	// TestJson test; // faild hien loi nhung build duoc
	TestTechnique test;

	test.run();
}
