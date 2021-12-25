#define __MT_MAIN_CPP__

#include "common.h"
#include "Game/Game.h"
#include "test/Test.h"

int main()
{
	#if 0 // ENGINE

	mt::game::Game program;
	program.run();

	#else // TEST

	mt::test::Test program;
	program.run();

	#endif

	return 0;
}
