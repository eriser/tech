#include "dispatcher_linux.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <tech/logger.h>


namespace Tech {


DispatcherImpl::DispatcherImpl() :
	epfd_(epoll_create(!0))
{
	if(epfd_ == -1)
		LOG("epoll_create call failed: {}", ::strerror(errno));
}


DispatcherImpl::~DispatcherImpl()
{
	close(epfd_);
}


bool DispatcherImpl::hasPendingEvents() const
{
	// FIXME
	return false;
}


void DispatcherImpl::processPendingEvents(int msecs)
{
	epoll_event events[16];

	int count = epoll_wait(epfd_, events, 16, msecs);
	if(count == -1) {
		LOG("epoll_wait call failed: {}", ::strerror(errno));
		return;
	}

	for(int i = 0; i < count; ++i) {
		int fd = events[i].data.fd;
		auto it = handlersByFd_.find(fd);
		u32 eventMask = events[i].events;

		// An event handler can unregister itself, thus we need to check its validity
		// every time wi use it
		if((eventMask & EPOLLIN) && isHandlerRegistered(fd))
			it->second(fd, EventType::kRead);

		if((eventMask & EPOLLOUT) && isHandlerRegistered(fd))
			it->second(fd, EventType::kWrite);

		if((eventMask & EPOLLRDHUP) && isHandlerRegistered(fd))
			it->second(fd, EventType::kClose);

		if((eventMask & EPOLLERR) && isHandlerRegistered(fd))
			it->second(fd, EventType::kError);
	}
}


bool DispatcherImpl::isHandlerRegistered(int fd) const
{
	return handlersByFd_.find(fd) != handlersByFd_.end();
}


bool DispatcherImpl::registerHandler(int fd, Flags<EventType> events,
		const EventHandler& handler)
{
	if(fd < 0 || !events) {
		LOG("Unable to register event handler for empty events.");
		return false;
	}

	auto it = handlersByFd_.find(fd);
	if(it != handlersByFd_.end()) {
		LOG("Event handler is already registered.");
		return false;
	}

	epoll_event ev;
	ev.data.fd = fd;
	ev.events = 0;

	if(events & EventType::kRead)
		ev.events |= EPOLLIN | EPOLLET;

	if(events & EventType::kWrite)
		ev.events |= EPOLLOUT | EPOLLET;

	if(events & EventType::kClose)
		ev.events |= EPOLLRDHUP | EPOLLET;

	if(events & EventType::kError)
		ev.events |= EPOLLERR | EPOLLET;

	if(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1 && errno != EEXIST) {
		LOG("epoll_ctl call failed: {}", ::strerror(errno));
		return false;
	}

	handlersByFd_.emplace(std::make_pair(fd, handler));
	return true;
}


bool DispatcherImpl::unregisterHandler(int fd)
{
	auto it = handlersByFd_.find(fd);
	if(it == handlersByFd_.end())
		return false;

	// Check that file descriptor is valid
	if(::fcntl(fd, F_GETFL) == -1) {
		if(errno != EBADF) {
			LOG("fcntl call failed: {}", ::strerror(errno));
			return false;
		}

		handlersByFd_.erase(it);
		return true;
	}

	epoll_event ev;
	ev.data.fd = fd;
	ev.events = 0;

	if(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &ev) == -1 && errno != ENOENT) {
		LOG("epoll_ctl call failed: {}", ::strerror(errno));
		return false;
	}

	handlersByFd_.erase(it);
	return true;
}


} // namespace Tech
