#ifndef TECH_SYSTEM_DISPATCHER_LINUX_H
#define TECH_SYSTEM_DISPATCHER_LINUX_H

#include <map>
#include <tech/system/dispatcher.h>


namespace Tech {


class DispatcherImpl {
public:
	DispatcherImpl();
	~DispatcherImpl();

	bool hasPendingEvents() const;
	void processPendingEvents(int msecs);

	bool isHandlerRegistered(int fd) const;
	bool registerHandler(int fd, Flags<EventType> events, const EventHandler& handler);
	bool unregisterHandler(int fd);

private:
	int epfd_;
	std::map<int, EventHandler> handlersByFd_;
};


} // namespace Tech


#endif // TECH_SYSTEM_DISPATCHER_LINUX_H
