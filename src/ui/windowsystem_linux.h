#ifndef TECH_UI_WINDOWSYSTEM_LINUX_H
#define TECH_UI_WINDOWSYSTEM_LINUX_H

#include <unordered_map>
#include <unordered_set>
#include <cairo.h>
#include <cairo-xcb.h>
#include <tech/string.h>
#include <tech/timecounter.h>
#include <tech/ui/events.h>
#include <tech/ui/timer.h>
#include <tech/ui/widget.h>


namespace Tech {


class WindowSystemPrivate {
public:
	WindowSystemPrivate();
	~WindowSystemPrivate();

	void sync();

	Widget::Handle createWindow(Widget* widget, Widget::Handle embedder);
	void destroyWindow(Widget::Handle handle);
	Widget* findWindow(Widget::Handle handle) const;

	cairo_surface_t* windowSurface(Widget::Handle handle) const;

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

	void processEvents();
	void stopEventProcessing();

private:
	xcb_connection_t* connection_;
	xcb_screen_t* screen_;
	int xcbFd_;
	int epollFd_;

	struct WindowData {
		Widget* widget;
		cairo_surface_t* surface;
		xcb_atom_t wmDeleteWindow;
	};

	std::unordered_map<Widget::Handle, WindowData> dataByHandle_;
	std::unordered_map<Timer::Handle, Timer*> timerByHandle_;
	MouseButtons mouseButtons_;

	enum class Command {
		kStopProcessing,
		kRepaintWidgets,
		kDeleteWidgets
	};

	int pipeFds_[2];
	std::unordered_set<Widget*> repaintQueue_;
	std::unordered_set<Widget*> deletionQueue_;

	void processWindowEvents();

	bool isHandleValid(Widget::Handle handle) const;

	const WindowData* dataByHandle(Widget::Handle handle) const;
	Timer* timerByHandle(Timer::Handle handle);
	static MouseButton translateMouseButton(u8 button);
	static KeyModifiers translateKeyModifier(u16 state);
	static bool checkWheelEvent(u8 detail, bool* vertical, int* delta);
	static Key keycodeToKey(int keysym);
};


} // namespace Tech


#endif // TECH_UI_WINDOWSYSTEM_LINUX_H
