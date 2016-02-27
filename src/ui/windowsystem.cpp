#include "windowsystem.h"

#include <tech/platform.h>
#include PLATFORM_HEADER(windowsystem)
#include <tech/logger.h>


namespace Tech {


WindowSystem::WindowSystem() :
	Interface<WindowSystemPrivate>(new WindowSystemPrivate)
{
}


WindowSystem* WindowSystem::instance()
{
	static WindowSystem instance;
	return &instance;
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
		LOG("Unable to create window with invalid geometry");
		return Widget::kInvalidHandle;
	}

	return impl()->createWindow(widget, parent);
}


void WindowSystem::destroyWindow(Widget::Handle handle)
{
	if(handle == Widget::kInvalidHandle) {
		LOG("Unable to destroy invalid window");
		return;
	}

	impl()->destroyWindow(handle);
}


Widget* WindowSystem::findWindow(Widget::Handle handle) const
{
	if(handle == Widget::kInvalidHandle) {
		LOG("Unable to find invalid window");
		return nullptr;
	}

	return impl()->findWindow(handle);
}


void WindowSystem::setWindowSizeLimits(Widget::Handle handle, const Size<int>& minSize,
		const Size<int>& maxSize)
{
	if(handle == Widget::kInvalidHandle) {
		LOG("Unable to set size limits for invalid window");
		return;
	}

	return impl()->setWindowSizeLimits(handle, minSize, maxSize);
}


void WindowSystem::moveWindow(Widget::Handle handle, const Point<int>& pos)
{
	if(handle == Widget::kInvalidHandle) {
		LOG("Unable to move invalid window");
		return;
	}

	return impl()->moveWindow(handle, pos);
}


void WindowSystem::resizeWindow(Widget::Handle handle, const Size<int>& size)
{
	if(handle == Widget::kInvalidHandle) {
		LOG("Unable to resize invalid window");
		return;
	}

	return impl()->resizeWindow(handle, size);
}


void WindowSystem::setWindowVisible(Widget::Handle handle, bool visible)
{
	if(handle == Widget::kInvalidHandle) {
		LOG("Unable to change visibility of invalid window");
		return;
	}

	impl()->setWindowVisible(handle, visible);
}


void WindowSystem::setWindowFrameless(Widget::Handle handle, bool enabled)
{
	if(handle == Widget::kInvalidHandle) {
		LOG("Unable to make invalid window frameless");
		return;
	}

	impl()->setWindowFrameless(handle, enabled);
}


void WindowSystem::setWindowTaskbarButton(Widget::Handle handle, bool enabled)
{
	if(handle == Widget::kInvalidHandle) {
		LOG("Unable to remove taskbar button for invalid window");
		return;
	}

	impl()->setWindowTaskbarButton(handle, enabled);
}


void WindowSystem::setWindowTitle(Widget::Handle handle, const String& title)
{
	if(handle == Widget::kInvalidHandle) {
		LOG("Unable to set title of invalid window");
		return;
	}

	impl()->setWindowTitle(handle, title);
}


void WindowSystem::repaintWindow(Widget::Handle handle, const Rect<int>& rect)
{
	if(handle == Widget::kInvalidHandle) {
		LOG("Unable to repaint invalid window");
		return;
	}

	impl()->repaintWindow(handle, rect);
}


void WindowSystem::enqueueWidgetRepaint(Widget* widget)
{
	if(!widget) {
		LOG("Unable enqueue repainting for null widget");
		return;
	}

	impl()->enqueueWidgetRepaint(widget);
}


void WindowSystem::enqueueWidgetDeletion(Widget* widget)
{
	if(!widget) {
		LOG("Unable enqueue deletion of null widget");
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
		LOG("Unable destroy invalid timer");
		return;
	}

	impl()->destroyTimer(handle);
}


void WindowSystem::startTimer(Timer::Handle handle, Duration timeout, bool periodic)
{
	if(handle == Timer::kInvalidHandle) {
		LOG("Unable start invalid timer");
		return;
	}

	impl()->startTimer(handle, timeout, periodic);
}


void WindowSystem::stopTimer(Timer::Handle handle)
{
	if(handle == Timer::kInvalidHandle) {
		LOG("Unable stop invalid timer");
		return;
	}

	impl()->stopTimer(handle);
}


bool WindowSystem::isTimerActive(Timer::Handle handle) const
{
	if(handle == Timer::kInvalidHandle) {
		LOG("Unable get active status of invalid timer");
		return false;
	}

	return impl()->isTimerActive(handle);
}


Duration WindowSystem::timerInterval(Timer::Handle handle) const
{
	if(handle == Timer::kInvalidHandle) {
		LOG("Unable get interval of invalid timer");
		return Duration();
	}

	return impl()->timerInterval(handle);
}


} // namespace Tech
