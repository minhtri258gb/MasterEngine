#define __MT_TIMER_CPP__

#include "common.h"
#include "Timer.h"

using namespace mt::engine;

Timer Timer::ins;

Timer::Timer()
{
	frameTimeLimit = (long)(1000.0 / 60.0);
	lastTime = 0;
	elapsedTime = 0.0f;
}

Timer::~Timer()
{
}

void Timer::init()
{
	lastTime = clock();
}

void Timer::setFPSLimit(int value)
{
	frameTimeLimit = (long)(1000.0 / value);
}

int Timer::getFPS()
{
    return (elapsedTime != 0.0f) ? int(1.0f / elapsedTime) : 0;
}

void Timer::sleep()
{
	long nextTime = frameTimeLimit + lastTime;
	while (clock() < nextTime) { } // Sleep
	long curTime = clock();
    elapsedTime = (curTime - lastTime) / 1000.0f;
    lastTime = curTime;
}

float Timer::getTimePassed()
{
    return elapsedTime;
}
