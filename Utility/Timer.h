#ifndef TIMER_H
#define TIMER_H
#include <glad/glad.h>
#include <glfw3.h>
typedef unsigned int Uint32;
class Timer
{
public:
	//Initializes variables
	Timer();

	//The various clock actions
	void start();
	void stop();
	void pause();
	void unpause();

	//Gets the timer's time in Milliseconds.
	float getTicks();

	//Checks the status of the timer
	bool isStarted();
	bool isPaused();

private:
	//The clock time when the timer started
	float mStartTicks;

	//The ticks stored when the timer was paused
	float mPausedTicks;

	//The timer status
	bool mPaused;
	bool mStarted;
};


#endif 