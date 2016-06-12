#ifndef TECH_SYSTEM_TIMER_H
#define TECH_SYSTEM_TIMER_H

#include <tech/duration.h>
#include <tech/passkey.h>
#include <tech/pimpl.h>
#include <tech/signal.h>
#include <tech/types.h>


namespace Tech {


class TimerImpl;


class Timer final : public Trackable, public Interface<TimerImpl> {
public:
	using Handle = int;
	static const Handle kInvalidHandle = 0;

	Timer();
	Timer(const Timer& other) = delete;

	Timer& operator=(const Timer& other) = delete;

	Handle handle() const;
	Duration interval() const;
	bool isActive() const;

	void start(Duration timeout, bool periodic = false);
	void stop();

	// signal void timeout(Timer* sender)
	Signal<void(Timer* sender), PassKey<TimerImpl>> timeout;
};


} // namespace Tech


#endif // TECH_SYSTEM_TIMER_H
