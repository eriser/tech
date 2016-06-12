#ifndef TECH_SYSTEM_DISPATCHER_H
#define TECH_SYSTEM_DISPATCHER_H

#include <tech/delegate.h>
#include <tech/flags.h>
#include <tech/pimpl.h>


namespace Tech {


class DispatcherImpl;


enum class EventType {
	kRead  = 0x01,
	kWrite = 0x02,
	kClose = 0x04,
	kError = 0x08
};


using EventHandler = Delegate<void(int, EventType)>;


class Dispatcher : public Interface<DispatcherImpl> {
public:
	static Dispatcher* instance();

	Dispatcher(const Dispatcher&) = delete;
	Dispatcher& operator=(const Dispatcher&) = delete;

	bool hasPendingEvents() const;
	void processPendingEvents(int msecs = 0);

	bool isHandlerRegistered(int fd) const;
	bool registerHandler(int fd, Flags<EventType> events, const EventHandler& handler);
	bool unregisterHandler(int fd);

private:
	Dispatcher();
};


} // namespace Tech


DECLARE_FLAG_OPERATORS(Tech::EventType);


#endif // TECH_SYSTEM_DISPATCHER_H
