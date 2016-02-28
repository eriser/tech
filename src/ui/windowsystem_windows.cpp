#include "windowsystem_windows.h"

#include <windowsx.h>
#include <tech/logger.h>
#include <tech/ui/painter.h>


namespace Tech {


WindowSystemPrivate::WindowSystemPrivate()
{
    module_ = GetModuleHandle(nullptr);
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
    wclass.hInstance = module_;
    wclass.hIcon = LoadIcon(module_, kWindowClass);
	wclass.hCursor = LoadCursor(0, IDI_APPLICATION);
	wclass.lpszClassName = kWindowClass;

	if(!RegisterClassEx(&wclass)) {
		LOG("Unable to register window class: {0}", errorString());
		return Widget::kInvalidHandle;
	}

	RECT wr = { 0, 0, widget->width(), widget->height() };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

	HWND hwnd = CreateWindowEx(WS_EX_APPWINDOW, kWindowClass,
			widget->windowTitle().toUtf8(), WS_OVERLAPPEDWINDOW, widget->x(), widget->y(),
            wr.right - wr.left, wr.bottom - wr.top, 0, 0, module_, nullptr);

	if(!hwnd) {
		LOG("Unable to create window: %s", errorString().c_str());
        UnregisterClass(kWindowClass, module_);
		return Widget::kInvalidHandle;
	}

	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	Widget::Handle handle = reinterpret_cast<Widget::Handle>(hwnd);
    dataByHandle_.emplace(makePair(handle, WindowData{widget, false}));

	return reinterpret_cast<iptr>(hwnd);
}


void WindowSystemPrivate::destroyWindow(Widget::Handle handle)
{
	const WindowData* data = dataByHandle(handle);
	if(!data) {
		return;
	}

	DestroyWindow(reinterpret_cast<HWND>(handle));
    UnregisterClass(kWindowClass, module_);
	dataByHandle_.erase(handle);
}


Widget* WindowSystemPrivate::findWindow(Widget::Handle handle) const
{
	const WindowData* data = dataByHandle(handle);
	if(!data) {
		return nullptr;
	}

	return data->widget;
}


void WindowSystemPrivate::setWindowSizeLimits(Widget::Handle handle,
		const Size<int>& minSize, const Size<int>& maxSize)
{
	UNUSED(handle);
	UNUSED(minSize);
	UNUSED(maxSize);
}


void WindowSystemPrivate::moveWindow(Widget::Handle handle, const Point<int>& pos)
{
	HWND hwnd = reinterpret_cast<HWND>(handle);
    SetWindowPos(hwnd, 0, pos.x(), pos.y(), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}


void WindowSystemPrivate::resizeWindow(Widget::Handle handle, const Size<int>& size)
{
    RECT wr = { 0, 0, size.width(), size.height() };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

    HWND hwnd = reinterpret_cast<HWND>(handle);
    SetWindowPos(hwnd, 0, 0, 0, wr.right - wr.left, wr.bottom - wr.top,
            SWP_NOMOVE | SWP_NOZORDER);
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
	HWND hwnd = reinterpret_cast<HWND>(handle);
	SetWindowText(hwnd, title.toUtf8());
}


void WindowSystemPrivate::repaintWindow(Widget::Handle handle, const Rect<int>& rect)
{
	RECT r;
	r.top    = rect.top();
	r.left   = rect.left();
	r.right  = rect.right();
	r.bottom = rect.bottom();

	HWND hwnd = reinterpret_cast<HWND>(handle);
	RedrawWindow(hwnd, &r, nullptr, RDW_INTERNALPAINT);
}


void WindowSystemPrivate::enqueueWidgetRepaint(Widget* widget)
{
	Rect<int> rect = widget->rect();

	// Step down to the lowest widget (window)
	while(widget->parent()) {
		rect.translate(widget->pos());
		widget = widget->parent();
	}

	RECT r = { rect.left(), rect.top(), rect.right(), rect.bottom() };

    // FIXME
	HWND hwnd = reinterpret_cast<HWND>(widget->handle());
	InvalidateRect(hwnd, &r, false);
	UpdateWindow(hwnd);
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
	LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	WindowSystemPrivate* self = reinterpret_cast<WindowSystemPrivate*>(ptr);
	if(!self)
		return DefWindowProc(hwnd, message, wParam, lParam);

	Widget::Handle handle = reinterpret_cast<Widget::Handle>(hwnd);
	const WindowData* data = self->dataByHandle(handle);
	if(!data || !data->widget)
		return DefWindowProc(hwnd, message, wParam, lParam);

	Widget* widget = data->widget;

	switch(message) {
	case WM_CLOSE:
		ShowWindow(hwnd, SW_HIDE);
		return 0;

    case WM_SIZE: {
        Size<int> size(LOWORD(lParam), HIWORD(lParam));

        ResizeEvent e;
        e.setSize(size, {});
        e.setOldSize(widget->size(), {});
        widget->processEvent(&e, {});

//        widget->repaint();
        return 0; }

	case WM_MOUSEMOVE: {
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Point<int> pos(x, y);
//		Point<int> globalPos(ev->root_x, ev->root_y);

		if(!data->hasMouse) {
			LOG("mouse enter");

			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = hwnd;
			TrackMouseEvent(&tme);

			data->hasMouse = true;

			MouseEvent event(Event::kMouseEnter);
	//		event.setTimestamp(ev->time, {});
	//		event.setModifiers(translateKeyModifier(ev->state), {});
			event.setPos(pos, {});
			event.setWindowPos(pos, {});
	//		event.setGlobalPos(globalPos, {});
	//		event.setButtons(mouseButtons_, {});
			data->widget->processEvent(&event, {});

		}

		MouseEvent event(Event::kMouseMove);
//		event.setTimestamp(ev->time, {});
//		event.setModifiers(translateKeyModifier(ev->state), {});
		event.setPos(pos, {});
		event.setWindowPos(pos, {});
//		event.setGlobalPos(globalPos, {});
//		event.setButtons(mouseButtons_, {});
		data->widget->processEvent(&event, {});

		return 0; }

	case WM_MOUSELEAVE: {
		LOG("mouse leave");
		int x = GET_X_LPARAM(lParam);
		int y = GET_Y_LPARAM(lParam);

		Point<int> pos(x, y);
//		Point<int> globalPos(ev->root_x, ev->root_y);

		MouseEvent e(Event::kMouseLeave);
//		e.setTimestamp(ev->time, {});
//		e.setModifiers(translateKeyModifier(ev->state), {});
		e.setPos(pos, {});
		e.setWindowPos(pos, {});
//		e.setGlobalPos(globalPos, {});
//		e.setButtons(mouseButtons_, {});
		data->widget->processEvent(&e, {});
		data->hasMouse = false;

		return 0; }

	case WM_LBUTTONDOWN: {
        SetCapture(hwnd);
//		Point<int> pos(x, y);
//		Point<int> globalPos(ev->root_x, ev->root_y);

		MouseEvent e(Event::kMousePress);
//		e.setTimestamp(ev->time, {});
//		e.setModifiers(translateKeyModifier(ev->state), {});
//		e.setPos(pos, {});
//		e.setWindowPos(pos, {});
//		e.setGlobalPos(globalPos, {});

		e.setButton(MouseButton::kLeft, {});
		data->widget->processEvent(&e, {});
		return 0; }

	case WM_LBUTTONUP: {
        ReleaseCapture();
//		Point<int> pos(x, y);
//		Point<int> globalPos(ev->root_x, ev->root_y);

		MouseEvent e(Event::kMouseRelease);
//		e.setTimestamp(ev->time, {});
//		e.setModifiers(translateKeyModifier(ev->state), {});
//		e.setPos(pos, {});
//		e.setWindowPos(pos, {});
//		e.setGlobalPos(globalPos, {});

		e.setButton(MouseButton::kLeft, {});
		data->widget->processEvent(&e, {});
		return 0; }

	case WM_RBUTTONDOWN: {
        SetCapture(hwnd);
        return 0; }

	case WM_RBUTTONUP: {
        ReleaseCapture();
        return 0; }

	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		cairo_surface_t* surface = cairo_win32_surface_create(hdc);
		cairo_t* cr = cairo_create(surface);

        cairo_surface_t* dibSurface = cairo_win32_surface_create_with_dib(
                CAIRO_FORMAT_RGB24, widget->width(), widget->height());

		RECT r;
		GetUpdateRect(hwnd, &r, FALSE);

		Rect<int> rect;
		rect.setTopLeft(r.left, r.top);
		rect.setBottomRight(r.right, r.bottom);

        Painter painter(dibSurface, {});

		PaintEvent event;
        event.setPainter(&painter, {});

		if(rect.isNull()) {
			event.setRect(widget->rect(), {});
		}
		else {
			event.setRect(rect, {});
		}

		widget->processEvent(&event, {});

        cairo_set_source_surface(cr, dibSurface, 0, 0);
		cairo_paint(cr);

        EndPaint(hwnd, &ps);
        cairo_destroy(cr);
        cairo_surface_finish(dibSurface);
        cairo_surface_finish(surface);
		return 0; }

	case WM_TIMER:
		break;

	case WM_GETMINMAXINFO: {
		MINMAXINFO* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
		mmi->ptMinTrackSize.x = widget->minimumWidth();
		mmi->ptMinTrackSize.y = widget->minimumHeight();
		mmi->ptMaxTrackSize.x = widget->maximumWidth();
		mmi->ptMaxTrackSize.y = widget->maximumHeight();
		return 0; }
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}


} // namespace Tech
