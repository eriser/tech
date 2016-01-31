#include <tech/ui/timer.h>

#include "windowsystem.h"

#define WINDOW_SYSTEM	WindowSystem::instance()


namespace Tech {


Timer::Timer() :
	handle_(WINDOW_SYSTEM->createTimer(this))
{
}


Timer::~Timer()
{
	WINDOW_SYSTEM->destroyTimer(handle_);
}


Timer::Handle Timer::handle() const
{
	return handle_;
}


Duration Timer::interval() const
{
	return WINDOW_SYSTEM->timerInterval(handle_);
}


bool Timer::isActive() const
{
	return WINDOW_SYSTEM->isTimerActive(handle_);
}


void Timer::start(Duration timeout, bool periodic)
{
	WINDOW_SYSTEM->startTimer(handle_, timeout, periodic);
}


void Timer::stop()
{
	WINDOW_SYSTEM->stopTimer(handle_);
}


} // namespace Tech
