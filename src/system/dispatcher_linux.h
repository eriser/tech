#ifndef TECH_SYSTEM_DISPATCHER_LINUX_H
#define TECH_SYSTEM_DISPATCHER_LINUX_H

#include <map>
#include <unordered_set>
#include <tech/system/dispatcher.h>


namespace Tech {


class DispatcherImpl {
public:
	DispatcherImpl();
	~DispatcherImpl();

	void processEvents();
	void stopProcessing();

	bool isHandlerRegistered(int fd) const;
	bool registerHandler(int fd, Flags<EventType> events, const EventHandler& handler);
	bool unregisterHandler(int fd);

	void addDeleter(Dispatcher::AbstractDeleter* deleter);

private:
	enum class Command {
		kStopProcessing
	};

	int epfd_;
	int pipeFds_[2];
	std::map<int, EventHandler> handlersByFd_;
	std::unordered_set<Box<Dispatcher::AbstractDeleter>> deleters_;
};


} // namespace Tech


#endif // TECH_SYSTEM_DISPATCHER_LINUX_H
