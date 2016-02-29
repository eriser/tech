#ifndef TECH_UI_WINDOWSYSTEM_WINDOWS_H
#define TECH_UI_WINDOWSYSTEM_WINDOWS_H

#include <unordered_map>
#include <unordered_set>
#include <windows.h>
#include <tech/string.h>
#include <tech/timecounter.h>
#include <tech/version.h>
#include <tech/ui/events.h>
#include <tech/ui/timer.h>
#include <tech/ui/widget.h>


namespace Tech {


class WindowSystemPrivate {
public:
	WindowSystemPrivate();

	~WindowSystemPrivate();

	void sync();

	Widget::Handle createWindow(Widget* widget, Widget::Handle owner);
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

	void processEvents();
	void stopEventProcessing();

private:
	static constexpr const char* kWindowClass = PROJECT_NAME;

	HMODULE module_;
	HWND commandHwnd_;
	UINT_PTR nextTimerId_;

	struct WindowData {
		Widget* widget;
		mutable bool hasMouse;
	};

	std::unordered_map <Widget::Handle, WindowData> dataByHandle_;
	std::unordered_map<Timer::Handle, Timer*> timerByHandle_;

	std::unordered_set<Widget*> repaintQueue_;
	std::unordered_set<Widget*> deletionQueue_;

	const WindowData* dataByHandle(Widget::Handle handle) const;
	static std::string errorString();

	static LRESULT CALLBACK	commandProc(HWND hwnd, UINT message, WPARAM wParam,
			LPARAM lParam);

	static LRESULT CALLBACK	windowProc(HWND hwnd, UINT message, WPARAM wParam,
			LPARAM lParam);
};


} // namespace Tech


#endif // TECH_UI_WINDOWSYSTEM_WINDOWS_H
