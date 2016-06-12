#ifndef TECH_SYSTEM_TIMER_LINUX_H
#define TECH_SYSTEM_TIMER_LINUX_H

#include <tech/pimpl.h>
#include <tech/types.h>
#include <tech/system/timer.h>


namespace Tech {


enum class EventType;
class Timer;


class TimerImpl : public Implementation<Timer> {
public:
	TimerImpl(Timer* iface);
	TimerImpl(const TimerImpl&) = delete;
	~TimerImpl();
	TimerImpl& operator=(const TimerImpl&) = delete;

	Timer::Handle handle() const;
	Duration interval() const;
	bool isActive() const;

	void start(Duration timeout, bool periodic = false);
	void stop();

private:
	Timer::Handle handle_;

	void handleTimeout(int handle, EventType event);
};


} // namespace Tech


#endif // TECH_SYSTEM_TIMER_LINUX_H
