#define __MT_SCREEN_CPP__

#include "common.h"
#include "Screen.h"

using namespace mt::engine;
using namespace mt::graphic;

Screen::Screen()
{
}

Screen::~Screen()
{
}

void Screen::init()
{
	this->proj = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
}
