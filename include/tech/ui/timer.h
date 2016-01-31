#ifndef TECH_UI_TIMER_H
#define TECH_UI_TIMER_H

#include <tech/duration.h>
#include <tech/passkey.h>
#include <tech/signal.h>
#include <tech/types.h>


namespace Tech {


class WindowSystemPrivate;


class Timer final : public Trackable {
public:
	using Handle = int;
	static const Handle kInvalidHandle = 0;

	Timer();
	Timer(const Timer& other) = delete;

	~Timer() override;

	Timer& operator=(const Timer& other) = delete;

	Handle handle() const;
	Duration interval() const;
	bool isActive() const;

	void start(Duration timeout, bool periodic = false);
	void stop();

signals:
	Signal<void(), PassKey<WindowSystemPrivate>> timeout;

private:
	Handle handle_;
};


} // namespace Tech


#endif // TECH_UI_TIMER_H
