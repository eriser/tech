#include <tech/system/timer.h>

#include <tech/platform.h>
#include PLATFORM_HEADER(timer)


namespace Tech {


Timer::Timer() :
	Interface<TimerImpl>(new TimerImpl(this))
{
}


Timer::Handle Timer::handle() const
{
	return impl()->handle();
}


Duration Timer::interval() const
{
	return impl()->interval();
}


bool Timer::isActive() const
{
	return impl()->isActive();
}


void Timer::start(Duration timeout, bool periodic)
{
	return impl()->start(timeout, periodic);
}


void Timer::stop()
{
	return impl()->stop();
}


} // namespace Tech
