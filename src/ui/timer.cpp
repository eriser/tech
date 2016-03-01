#include <tech/ui/timer.h>

#include "windowsystem.h"


namespace Tech {


Timer::Timer() :
	ws_(WindowSystem::instance()),
	handle_(ws_->createTimer(this))
{
}


Timer::~Timer()
{
	ws_->destroyTimer(handle_);
}


Timer::Handle Timer::handle() const
{
	return handle_;
}


Duration Timer::interval() const
{
	return ws_->timerInterval(handle_);
}


bool Timer::isActive() const
{
	return ws_->isTimerActive(handle_);
}


void Timer::start(Duration timeout, bool periodic)
{
	ws_->startTimer(handle_, timeout, periodic);
}


void Timer::stop()
{
	ws_->stopTimer(handle_);
}


} // namespace Tech
