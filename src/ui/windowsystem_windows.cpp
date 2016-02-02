#include "windowsystem_windows.h"

#include <tech/logger.h>


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
	WNDCLASSEX wclass;
	std::memset(&wclass, 0, sizeof(WNDCLASSEX));

	wclass.cbSize = sizeof(WNDCLASSEX);
	wclass.style = CS_HREDRAW | CS_VREDRAW;
	wclass.lpfnWndProc = windowProc;
	wclass.cbClsExtra = 0;
	wclass.cbWndExtra = 0;
	wclass.hInstance = GetModuleHandle(nullptr);
	wclass.hIcon = LoadIcon(GetModuleHandle(nullptr), kWindowClass);
	wclass.hCursor = LoadCursor(0, IDI_APPLICATION);
	wclass.lpszClassName = kWindowClass;

	if(!RegisterClassEx(&wclass)) {
		LOG("Unable to register window class: {0}", errorString());
		return Widget::kInvalidHandle;
	}

	HWND hwnd = CreateWindowEx(WS_EX_APPWINDOW, kWindowClass,
			widget->windowTitle().toUtf8(), WS_OVERLAPPEDWINDOW, widget->x(), widget->y(),
			widget->width(), widget->height(), 0, 0, GetModuleHandle(nullptr), 0);

	if(!hwnd) {
		LOG("Unable to create window: %s", errorString().c_str());
		UnregisterClass(kWindowClass, GetModuleHandle(nullptr));
		return Widget::kInvalidHandle;
	}

	HDC dc = GetDC(hwnd);
	cairo_surface_t* surface = cairo_win32_surface_create(dc);

	ReleaseDC(hwnd, dc);

	if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
		LOG("Unable to create cairo Win32 surface");
		DestroyWindow(hwnd);
		UnregisterClass(kWindowClass, GetModuleHandle(nullptr));
		return Widget::kInvalidHandle;
	}

	Widget::Handle handle = reinterpret_cast<Widget::Handle>(hwnd);
	dataByHandle_.emplace(makePair(handle, WindowData{widget, surface}));

	return reinterpret_cast<iptr>(hwnd);
}


void WindowSystemPrivate::destroyWindow(Widget::Handle handle)
{
	const WindowData* data = dataByHandle(handle);
	if(!data) {
		return;
	}

	cairo_surface_finish(data->surface);
	DestroyWindow(reinterpret_cast<HWND>(handle));
	UnregisterClass(kWindowClass, GetModuleHandle(nullptr));
}


Widget* WindowSystemPrivate::findWindow(Widget::Handle handle) const
{
	const WindowData* data = dataByHandle(handle);
	if(!data) {
		return nullptr;
	}

	return data->widget;
}


cairo_surface_t* WindowSystemPrivate::windowSurface(Widget::Handle handle) const
{
	const WindowData* data = dataByHandle(handle);
	if(!data) {
		LOG("Unable to get surface of window with handle={0:#08X}", handle);
		return nullptr;
	}

	return data->surface;
}


void WindowSystemPrivate::setWindowSizeLimits(Widget::Handle handle,
		const Size<int>& minSize, const Size<int>& maxSize)
{

}


void WindowSystemPrivate::moveWindow(Widget::Handle handle, const Point<int>& pos)
{
	const WindowData* data = dataByHandle(handle);
	if(!data) {
		return;
	}

	HWND hwnd = reinterpret_cast<HWND>(handle);

	MoveWindow(hwnd, pos.x(), pos.y(), data->widget->width(), data->widget->height(),
			FALSE);
}


void WindowSystemPrivate::resizeWindow(Widget::Handle handle, const Size<int>& size)
{
	const WindowData* data = dataByHandle(handle);
	if(!data) {
		return;
	}

	HWND hwnd = reinterpret_cast<HWND>(handle);

	MoveWindow(hwnd, data->widget->x(), data->widget->y(), size.width(), size.height(),
			FALSE);
}


void WindowSystemPrivate::setWindowVisible(Widget::Handle handle, bool visible)
{
	HWND hwnd = reinterpret_cast<HWND>(handle);
	if(visible) {
		ShowWindow(hwnd, SW_SHOWNORMAL);
		UpdateWindow(hwnd);
	}
	else {
		ShowWindow(hwnd, SW_HIDE);
	}
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


void WindowSystemPrivate::startTimer(Timer::Handle handle, Duration timeout,
		bool periodic)
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
	MSG message;

	while(GetMessage(&message, nullptr, 0, 0) > 0) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}


void WindowSystemPrivate::stopEventProcessing()
{

}


const WindowSystemPrivate::WindowData*
WindowSystemPrivate::dataByHandle(Widget::Handle handle) const
{
	auto it = dataByHandle_.find(handle);
	if(it == dataByHandle_.end())
		return nullptr;

	return &it->second;
}


std::string WindowSystemPrivate::errorString()
{
	DWORD error = GetLastError();
	if(!error)
		return std::string();

	LPVOID buffer;
	DWORD length;
	length = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
						   FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&buffer),
			0, nullptr);

	if(length) {
		LPCSTR message = static_cast<LPCSTR>(buffer);
		std::string result(message, message + length);

		LocalFree(buffer);
		return result;
	}

	return std::string();
}


LRESULT CALLBACK WindowSystemPrivate::windowProc(HWND hwnd, UINT message, WPARAM
		wParam, LPARAM lParam)
{
	switch(message) {
	case WM_CLOSE:
		ShowWindow(hwnd, SW_HIDE);
		return 0;

	case WM_TIMER:
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}


} // namespace Tech
