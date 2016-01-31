#include "windowsystem.h"

#include <tech/platform.h>
#include PLATFORM_HEADER(windowsystem)


namespace Tech {


WindowSystem::WindowSystem() :
	Interface<WindowSystemPrivate>(new WindowSystemPrivate)
{
}


void WindowSystem::processEvents()
{
	impl()->processEvents();
}


void WindowSystem::stopEventProcessing()
{
	impl()->stopEventProcessing();
}


void WindowSystem::sync()
{
	impl()->sync();
}


Widget::Handle WindowSystem::createWindow(Widget* widget, Widget::Handle parent)
{
	if(!widget->geometry().isValid()) {
		return Widget::kInvalidHandle;
	}

	return impl()->createWindow(widget, parent);
}


void WindowSystem::destroyWindow(Widget::Handle handle)
{
	if(handle == Widget::kInvalidHandle) {
		return;
	}

	impl()->destroyWindow(handle);
}


WindowSystem* WindowSystem::instance()
{
	static WindowSystem instance;
	return &instance;
}


Widget* WindowSystem::findWindow(Widget::Handle handle) const
{
	if(handle == Widget::kInvalidHandle) {
		return nullptr;
	}

	return impl()->findWindow(handle);
}


cairo_surface_t* WindowSystem::windowSurface(Widget::Handle handle) const
{
	if(handle == Widget::kInvalidHandle) {
		return nullptr;
	}

	return impl()->windowSurface(handle);
}


void WindowSystem::setWindowSizeLimits(Widget::Handle handle, const Size<int>& minSize,
		const Size<int>& maxSize)
{
	if(handle == Widget::kInvalidHandle) {
		return;
	}

	return impl()->setWindowSizeLimits(handle, minSize, maxSize);
}


void WindowSystem::moveWindow(Widget::Handle handle, const Point<int>& pos)
{
	if(handle == Widget::kInvalidHandle) {
		return;
	}

	return impl()->moveWindow(handle, pos);
}


void WindowSystem::resizeWindow(Widget::Handle handle, const Size<int>& size)
{
	if(handle == Widget::kInvalidHandle) {
		return;
	}

	return impl()->resizeWindow(handle, size);
}


void WindowSystem::setWindowVisible(Widget::Handle handle, bool visible)
{
	if(handle == Widget::kInvalidHandle) {
		return;
	}

	impl()->setWindowVisible(handle, visible);
}


void WindowSystem::setWindowFrameless(Widget::Handle handle, bool enabled)
{
	if(handle == Widget::kInvalidHandle) {
		return;
	}

	impl()->setWindowFrameless(handle, enabled);
}


void WindowSystem::setWindowTaskbarButton(Widget::Handle handle, bool enabled)
{
	if(handle == Widget::kInvalidHandle) {
		return;
	}

	impl()->setWindowTaskbarButton(handle, enabled);
}


void WindowSystem::setWindowTitle(Widget::Handle handle, const String& title)
{
	if(handle == Widget::kInvalidHandle) {
		return;
	}

	impl()->setWindowTitle(handle, title);
}


void WindowSystem::enqueueWidgetRepaint(Widget* widget)
{
	if(!widget) {
		return;
	}

	impl()->enqueueWidgetRepaint(widget);
}


void WindowSystem::enqueueWidgetDeletion(Widget* widget)
{
	if(!widget) {
		return;
	}

	impl()->enqueueWidgetDeletion(widget);
}


Timer::Handle WindowSystem::createTimer(Timer* timer)
{
	return impl()->createTimer(timer);
}


void WindowSystem::destroyTimer(Timer::Handle handle)
{
	if(handle == Timer::kInvalidHandle) {
		return;
	}

	impl()->destroyTimer(handle);
}


void WindowSystem::startTimer(Timer::Handle handle, Duration timeout, bool periodic)
{
	if(handle == Timer::kInvalidHandle) {
		return;
	}

	impl()->startTimer(handle, timeout, periodic);
}


void WindowSystem::stopTimer(Timer::Handle handle)
{
	if(handle == Timer::kInvalidHandle) {
		return;
	}

	impl()->stopTimer(handle);
}


bool WindowSystem::isTimerActive(Timer::Handle handle) const
{
	if(handle == Timer::kInvalidHandle) {
		return false;
	}

	return impl()->isTimerActive(handle);
}


Duration WindowSystem::timerInterval(Timer::Handle handle) const
{
	if(handle == Timer::kInvalidHandle) {
		return Duration();
	}

	return impl()->timerInterval(handle);
}

} // namespace Tech
