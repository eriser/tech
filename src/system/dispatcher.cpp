#include <tech/system/dispatcher.h>

#include <tech/platform.h>
#include PLATFORM_HEADER(dispatcher)


namespace Tech {


Dispatcher::Dispatcher() :
	Tech::Interface<DispatcherImpl>(new DispatcherImpl)
{
}


Dispatcher* Dispatcher::instance()
{
	static thread_local Dispatcher dispatcher;
	return &dispatcher;
}


bool Dispatcher::hasPendingEvents() const
{
	return impl()->hasPendingEvents();
}


void Dispatcher::processPendingEvents(int msecs)
{
	impl()->processPendingEvents(msecs);
}


bool Dispatcher::isHandlerRegistered(int fd) const
{
	return impl()->isHandlerRegistered(fd);
}


bool Dispatcher::registerHandler(int fd, Flags<EventType> events,
		const EventHandler& handler)
{
	return impl()->registerHandler(fd, events, handler);
}


bool Dispatcher::unregisterHandler(int fd)
{
	return impl()->unregisterHandler(fd);
}


} // namespace Tech
