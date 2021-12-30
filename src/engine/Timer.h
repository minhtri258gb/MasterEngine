#ifndef __MT_TIMER_H__
#define __MT_TIMER_H__

namespace mt {
namespace engine {

class Timer
{
public:

	// Variables
	static Timer ins;

	// Forwards
	Timer();
	~Timer();

	// General
	void init();
	void sleep();

	// Get / Set
	void setFPSLimit(int);
	int getFPS();
	float getTimePassed();

private:

	// Variables
	long frameTimeLimit;
	long lastTime;
	float elapsedTime;
};

}}

#endif
