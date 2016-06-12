#include <tech/system/timer.h>

#include <tech/platform.h>
#include PLATFORM_HEADER(timer)


namespace Tech {


Timer::Timer() :
	Interface<TimerImpl>(new TimerImpl(this))
{
}


Timer::~Timer()
{
}


Timer::Handle Timer::handle() const
{
}


Duration Timer::interval() const
{
}


bool Timer::isActive() const
{
}


void Timer::start(Duration timeout, bool periodic)
{
}


void Timer::stop()
{
}


} // namespace Tech
