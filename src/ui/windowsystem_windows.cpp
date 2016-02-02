#include "windowsystem_windows.h"


namespace Tech {


WindowSystemPrivate::WindowSystemPrivate()
{

}


WindowSystemPrivate::~WindowSystemPrivate()
{

}


void WindowSystemPrivate::sync()
{

}


Widget::Handle WindowSystemPrivate::createWindow(Widget* widget, Widget::Handle owner)
{
	// FIXME
	return Widget::kInvalidHandle;
}


void WindowSystemPrivate::destroyWindow(Widget::Handle handle)
{

}


Widget* WindowSystemPrivate::findWindow(Widget::Handle handle) const
{
	// FIXME
	return nullptr;
}


cairo_surface_t* WindowSystemPrivate::windowSurface(Widget::Handle handle) const
{
	// FIXME
	return nullptr;
}


void WindowSystemPrivate::setWindowSizeLimits(Widget::Handle handle, const Size<int>& minSize,
		const Size<int>& maxSize)
{

}


void WindowSystemPrivate::moveWindow(Widget::Handle handle, const Point<int>& pos)
{

}


void WindowSystemPrivate::resizeWindow(Widget::Handle handle, const Size<int>& size)
{

}


void WindowSystemPrivate::setWindowVisible(Widget::Handle handle, bool visible)
{

}


void WindowSystemPrivate::setWindowFrameless(Widget::Handle handle, bool enabled)
{

}


void WindowSystemPrivate::setWindowTaskbarButton(Widget::Handle handle, bool enabled)
{

}


void WindowSystemPrivate::setWindowTitle(Widget::Handle handle, const String& title)
{

}


void WindowSystemPrivate::enqueueWidgetRepaint(Widget* widget)
{

}


void WindowSystemPrivate::enqueueWidgetDeletion(Widget* widget)
{

}


Timer::Handle WindowSystemPrivate::createTimer(Timer* timer)
{
	// FIXME
	return Timer::kInvalidHandle;
}


void WindowSystemPrivate::destroyTimer(Timer::Handle handle)
{

}


void WindowSystemPrivate::startTimer(Timer::Handle handle, Duration timeout, bool periodic)
{

}


void WindowSystemPrivate::stopTimer(Timer::Handle handle)
{

}


bool WindowSystemPrivate::isTimerActive(Timer::Handle handle) const
{
	return false;
}


Duration WindowSystemPrivate::timerInterval(Timer::Handle handle) const
{
	// FIXME
	return Duration();
}


void WindowSystemPrivate::processEvents()
{

}


void WindowSystemPrivate::stopEventProcessing()
{

}


} // namespace Tech
