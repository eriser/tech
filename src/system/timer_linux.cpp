#include "timer_linux.h"

#include <unistd.h>
#include <sys/timerfd.h>

#include <tech/logger.h>
#include <tech/system/dispatcher.h>


namespace Tech {


TimerImpl::TimerImpl(Timer* iface) :
	Implementation<Timer>(iface)
{
	handle_ = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	if(handle_ == -1) {
		LOG("timerfd_create failed");
		handle_ = Timer::kInvalidHandle;
	}

	Delegate<void(int, EventType)> handler(this, &TimerImpl::handleTimeout);
	Dispatcher::instance()->registerHandler(handle_, EventType::kRead, handler);
}


TimerImpl::~TimerImpl()
{
	Dispatcher::instance()->deregisterHandler(handle_);
	close(handle_);
}


Timer::Handle TimerImpl::handle() const
{
	return handle_;
}


Duration TimerImpl::interval() const
{
	itimerspec data;

	if(timerfd_gettime(handle_, &data) != 0)
		return Duration();

	return Duration(data.it_interval.tv_sec * 1000 + data.it_interval.tv_nsec / 1000000);
}


bool TimerImpl::isActive() const
{
	itimerspec data;

	if(timerfd_gettime(handle_, &data) != 0)
		return false;

	return data.it_interval.tv_nsec != 0 && data.it_value.tv_nsec != 0 &&
		   data.it_interval.tv_sec != 0 && data.it_value.tv_sec != 0;
}


void TimerImpl::start(Duration timeout, bool periodic)
{
	itimerspec data;
	std::memset(&data, 0, sizeof(data));

	data.it_value.tv_sec = timeout.seconds();
	data.it_value.tv_nsec = (timeout.mseconds() - data.it_value.tv_sec * 1000) * 1000000;

	if(periodic)
		data.it_interval = data.it_value;

	timerfd_settime(handle_, 0, &data, nullptr);
}


void TimerImpl::stop()
{
	itimerspec data;
	std::memset(&data, 0, sizeof(data));
	timerfd_settime(handle_, 0, &data, nullptr);
}


void TimerImpl::handleTimeout(int handle, EventType event)
{
	if(handle == handle_ && event == EventType::kRead) {
		u64 value;
		read(handle_, &value, sizeof(value));

		iface()->timeout({}, iface());
	}
}


} // namespace Tech
