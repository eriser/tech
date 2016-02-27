#ifndef TECH_UI_WINDOWSYSTEM_H
#define TECH_UI_WINDOWSYSTEM_H

#include <tech/pimpl.h>
#include <tech/string.h>
#include <tech/types.h>
#include <tech/ui/rect.h>
#include <tech/ui/timer.h>
#include <tech/ui/widget.h>


namespace Tech {


class WindowSystemPrivate;


class WindowSystem final : public Interface<WindowSystemPrivate> {
public:
	static WindowSystem* instance();

	void processEvents();
	void stopEventProcessing();

	void sync();

	Widget::Handle createWindow(Widget* widget,
			Widget::Handle parent = Widget::kInvalidHandle);

	void destroyWindow(Widget::Handle handle);
	Widget* findWindow(Widget::Handle handle) const;

	void setWindowSizeLimits(Widget::Handle handle, const Size<int>& minSize,
			const Size<int>& maxSize);

	void moveWindow(Widget::Handle handle, const Point<int>& pos);
	void resizeWindow(Widget::Handle handle, const Size<int>& size);

	void setWindowVisible(Widget::Handle handle, bool visible);
	void setWindowFrameless(Widget::Handle handle, bool enabled);
	void setWindowTaskbarButton(Widget::Handle handle, bool enabled);
	void setWindowTitle(Widget::Handle handle, const String& title);

	void repaintWindow(Widget::Handle handle, const Rect<int>& rect);

	void enqueueWidgetRepaint(Widget* widget);
	void enqueueWidgetDeletion(Widget* widget);

	Timer::Handle createTimer(Timer* timer);
	void destroyTimer(Timer::Handle handle);

	void startTimer(Timer::Handle handle, Duration timeout, bool periodic);
	void stopTimer(Timer::Handle handle);

	bool isTimerActive(Timer::Handle handle) const;
	Duration timerInterval(Timer::Handle handle) const;

private:
	WindowSystem();
};


} // namespace Tech


#endif // TECH_UI_WINDOWSYSTEM_H
