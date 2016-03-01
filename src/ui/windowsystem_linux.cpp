#include "windowsystem_linux.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <tech/logger.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb.h>
#include <tech/ui/painter.h>


namespace Tech {


WindowSystemPrivate::WindowSystemPrivate()
{
	connection_ = xcb_connect(nullptr, nullptr);
	if(xcb_connection_has_error(connection_) != 0) {
		LOG("Unable to initialize XCB widget system: cannot open display");
		connection_ = nullptr;
		return;
	}

	screen_ = xcb_setup_roots_iterator(xcb_get_setup(connection_)).data;
	xcbFd_ = xcb_get_file_descriptor(connection_);

	epollFd_ = epoll_create(1);
	if(epollFd_ == -1) {
		LOG("Unable to initialize XCB widget system: cannot create epoll instance");
		xcb_disconnect(connection_);
		return;
	}

	epoll_event ev;
	ev.data.fd = xcbFd_;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLET;

	if(epoll_ctl(epollFd_, EPOLL_CTL_ADD, xcbFd_, &ev) == -1 && errno != EEXIST) {
		LOG("epoll_ctl call failed: {0}", ::strerror(errno));
		close(epollFd_);
		xcb_disconnect(connection_);
		return;
	}

	pipe(pipeFds_);

	int flags = fcntl(pipeFds_[0], F_GETFL, 0);
	fcntl(pipeFds_[1], F_SETFL, flags | O_NONBLOCK);

	ev.data.fd = pipeFds_[0];
	ev.events = EPOLLIN;
	if(epoll_ctl(epollFd_, EPOLL_CTL_ADD, pipeFds_[0], &ev) == -1 && errno != EEXIST) {
		LOG("epoll_ctl call failed: {0}", ::strerror(errno));
		close(epollFd_);
		xcb_disconnect(connection_);
		return;
	}
}


WindowSystemPrivate::~WindowSystemPrivate()
{
	if(epollFd_ != -1)
		close(epollFd_);

	if(connection_)
		xcb_disconnect(connection_);
}


void WindowSystemPrivate::sync()
{
	xcb_flush(connection_);
}


Widget::Handle WindowSystemPrivate::createWindow(Widget* widget, Widget::Handle embedder)
{
	WindowFlags flags = widget->windowFlags();

	u32 mask = XCB_CW_BACK_PIXMAP
			   | XCB_CW_BIT_GRAVITY
			   | XCB_CW_WIN_GRAVITY
			   | XCB_CW_OVERRIDE_REDIRECT
			   | XCB_CW_SAVE_UNDER
			   | XCB_CW_EVENT_MASK;

	u32 values[] = {
			// XCB_CW_BACK_PIXMAP value:
			XCB_BACK_PIXMAP_NONE,

			// XCB_CW_BIT_GRAVITY value:
			XCB_GRAVITY_NORTH_WEST,

			// XCB_CW_WIN_GRAVITY value:
			XCB_GRAVITY_NORTH_WEST,

			// XCB_CW_OVERRIDE_REDIRECT value:
			flags & WindowFlag::kFrameless || embedder != Widget::kInvalidHandle,

			// XCB_CW_SAVE_UNDER value:
			0,

			// XCB_CW_EVENT_MASK values:
			XCB_EVENT_MASK_EXPOSURE
			| XCB_EVENT_MASK_STRUCTURE_NOTIFY
			| XCB_EVENT_MASK_KEY_PRESS
			| XCB_EVENT_MASK_KEY_RELEASE
			| XCB_EVENT_MASK_BUTTON_PRESS
			| XCB_EVENT_MASK_BUTTON_RELEASE
			| XCB_EVENT_MASK_BUTTON_MOTION
			| XCB_EVENT_MASK_ENTER_WINDOW
			| XCB_EVENT_MASK_LEAVE_WINDOW
			| XCB_EVENT_MASK_POINTER_MOTION
			| XCB_EVENT_MASK_PROPERTY_CHANGE
			| XCB_EVENT_MASK_FOCUS_CHANGE
	};

	usleep(10000);

	Widget::Handle parent = embedder;
	if(embedder == Widget::kInvalidHandle)
		parent = screen_->root;

	Widget::Handle handle = xcb_generate_id(connection_);

	auto c = xcb_create_window_checked(connection_, XCB_COPY_FROM_PARENT, handle, parent,
			static_cast<i16>(widget->x()), static_cast<i16>(widget->y()),
			static_cast<u16>(widget->width()), static_cast<u16>(widget->height()), 0,
			XCB_WINDOW_CLASS_INPUT_OUTPUT, screen_->root_visual, mask, values);

	auto error = xcb_request_check(connection_, c);
	if(error) {
		LOG("Unable to create XCB window: {0}", (int) error->error_code);
		return Widget::kInvalidHandle;
	}

	auto cookie = xcb_intern_atom(connection_, 1, 12, "WM_PROTOCOLS");
	auto* reply = xcb_intern_atom_reply(connection_, cookie, 0);

	cookie = xcb_intern_atom(connection_, 0, 16, "WM_DELETE_WINDOW");
	auto* reply2 = xcb_intern_atom_reply(connection_, cookie, 0);

	xcb_change_property(connection_, XCB_PROP_MODE_REPLACE, handle, reply->atom, 4, 32, 1,
			&reply2->atom);

	xcb_visualtype_t* visual = xcb_depth_visuals_iterator(
			(xcb_screen_allowed_depths_iterator(screen_)).data).data;

	cairo_surface_t* surface = cairo_xcb_surface_create(connection_, handle, visual,
			widget->width(), widget->height());

	if(cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
		LOG("Unable to create cairo XCB surface");
		xcb_destroy_window(connection_, handle);
		return Widget::kInvalidHandle;
	}

	dataByHandle_.emplace(makePair(handle, WindowData{ widget, surface, reply2->atom }));

	free(reply);
	free(reply2);
	return handle;
}


void WindowSystemPrivate::destroyWindow(Widget::Handle handle)
{
	const WindowData* data = dataByHandle(handle);
	if(!data) {
		return;
	}

	cairo_surface_finish(data->surface);
	xcb_destroy_window(connection_, handle);
	dataByHandle_.erase(handle);
}


Widget* WindowSystemPrivate::findWindow(Widget::Handle handle) const
{
	const WindowData* data = dataByHandle(handle);
	if(!data) {
		LOG("Unable to find window with handle={0:#08X}", handle);
		return nullptr;
	}

	return data->widget;
}


void WindowSystemPrivate::setWindowSizeLimits(Widget::Handle handle,
		const Size<int>& minSize, const Size<int>& maxSize)
{
	xcb_size_hints_t hints;
	hints.flags      = XCB_ICCCM_SIZE_HINT_P_MIN_SIZE | XCB_ICCCM_SIZE_HINT_P_MAX_SIZE;
	hints.min_width  = minOf(minSize.width(),  16535);
	hints.min_height = minOf(minSize.height(), 16535);
	hints.max_width  = minOf(maxSize.width(),  16535);
	hints.max_height = minOf(maxSize.height(), 16535);

	xcb_icccm_set_wm_size_hints(connection_, handle, XCB_ATOM_WM_NORMAL_HINTS, &hints);
}


void WindowSystemPrivate::moveWindow(Widget::Handle handle, const Point<int>& pos)
{
	u16 mask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y;
	i32 values[] = { pos.x(), pos.y() };
	xcb_configure_window(connection_, handle, mask, reinterpret_cast<u32*>(values));
}


void WindowSystemPrivate::resizeWindow(Widget::Handle handle, const Size<int>& size)
{
	u16 mask = XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT;
	i32 values[] = { size.width(), size.height() };
	xcb_configure_window(connection_, handle, mask, reinterpret_cast<u32*>(values));
}


void WindowSystemPrivate::setWindowVisible(Widget::Handle handle, bool visible)
{
	if(visible) {
		xcb_map_window(connection_, handle);
	}
	else {
		xcb_unmap_window(connection_, handle);
	}
}


void WindowSystemPrivate::setWindowFrameless(Widget::Handle handle, bool enabled)
{
	u32 mask = XCB_CW_OVERRIDE_REDIRECT;
	u32 values[] = { static_cast<u32>(enabled) };

	xcb_void_cookie_t cookie = xcb_change_window_attributes(connection_, handle, mask,
			values);

	xcb_generic_error_t* error = xcb_request_check(connection_, cookie);
	if(error) {
		return;
	}
}


void WindowSystemPrivate::setWindowTaskbarButton(Widget::Handle handle, bool enabled)
{
	auto cookie = xcb_intern_atom(connection_, 0, 13, "_NET_WM_STATE");
	auto reply = xcb_intern_atom_reply(connection_, cookie, 0);

	if(enabled) {
		auto cookie2 = xcb_intern_atom(connection_, 0, 26, "_NET_WM_STATE_SKIP_TASKBAR");
		auto reply2 = xcb_intern_atom_reply(connection_, cookie2, 0);

		xcb_change_property(connection_, XCB_PROP_MODE_REPLACE, handle, reply->atom,
				XCB_ATOM_ATOM, XCB_ATOM_VISUALID, 1, &(reply2->atom));

		free(reply2);
	}
	else {
		xcb_delete_property(connection_, handle, reply->atom);
	}

	free(reply);
}


void WindowSystemPrivate::setWindowTitle(Widget::Handle handle, const String& title)
{
	xcb_change_property(connection_,
			XCB_PROP_MODE_REPLACE,
			handle,
			XCB_ATOM_WM_NAME,
			XCB_ATOM_STRING,
			8,
			static_cast<u32>(title.length()),
			title.toUtf8());

	xcb_change_property(connection_,
			XCB_PROP_MODE_REPLACE,
			handle,
			XCB_ATOM_WM_ICON_NAME,
			XCB_ATOM_STRING,
			8,
			static_cast<u32>(title.length()),
			title.toUtf8());
}


void WindowSystemPrivate::repaintWindow(Widget::Handle handle, const Rect<int>& rect)
{
	const WindowData* data = dataByHandle(handle);
	if(!data) {
		LOG("Unable to repaint unknown window with handle={0:#08X}", handle);
		return;
	}

	Painter painter(data->surface, {});

	PaintEvent event;
	event.setPainter(&painter, {});
	event.setRect(rect, {});
	data->widget->processEvent(&event, {});
}


void WindowSystemPrivate::enqueueWidgetRepaint(Widget* widget)
{
	repaintQueue_.insert(widget);

	if(repaintQueue_.size() == 1) {
		Command command = Command::kRepaintWidgets;
		write(pipeFds_[1], &command, sizeof(command));
	}
}


void WindowSystemPrivate::enqueueWidgetDeletion(Widget* widget)
{
	deletionQueue_.insert(widget);

	if(deletionQueue_.size() == 1) {
		Command command = Command::kDeleteWidgets;
		write(pipeFds_[1], &command, sizeof(command));
	}
}


Timer::Handle WindowSystemPrivate::createTimer(Timer* timer)
{
	Timer::Handle handle = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
	if(handle == -1) {
		LOG("timerfd_create failed");
		return Timer::kInvalidHandle;
	}

	epoll_event ev;
	ev.data.fd = handle;
	ev.events = EPOLLIN | EPOLLET;

	if(epoll_ctl(epollFd_, EPOLL_CTL_ADD, handle, &ev) == -1 && errno != EEXIST) {
		LOG("epoll_ctl call failed: %s", ::strerror(errno));
		close(handle);
		return Timer::kInvalidHandle;
	}

	timerByHandle_.emplace(makePair(handle, timer));
	return handle;
}


void WindowSystemPrivate::destroyTimer(Timer::Handle handle)
{
	timerByHandle_.erase(handle);
	close(handle);
}


void WindowSystemPrivate::startTimer(Timer::Handle handle, Duration timeout,
		bool periodic)
{
	itimerspec data;
	std::memset(&data, 0, sizeof(data));

	data.it_value.tv_sec = timeout.seconds();
	data.it_value.tv_nsec = (timeout.mseconds() - data.it_value.tv_sec * 1000) * 1000000;

	if(periodic)
		data.it_interval = data.it_value;

	timerfd_settime(handle, 0, &data, nullptr);
}


void WindowSystemPrivate::stopTimer(Timer::Handle handle)
{
	itimerspec data;
	std::memset(&data, 0, sizeof(data));
	timerfd_settime(handle, 0, &data, nullptr);
}


bool WindowSystemPrivate::isTimerActive(Timer::Handle handle) const
{
	itimerspec data;

	if(timerfd_gettime(handle, &data) != 0)
		return false;

	return data.it_interval.tv_nsec != 0 && data.it_value.tv_nsec != 0 &&
		   data.it_interval.tv_sec != 0 && data.it_value.tv_sec != 0;
}


Duration WindowSystemPrivate::timerInterval(Timer::Handle handle) const
{
	itimerspec data;

	if(timerfd_gettime(handle, &data) != 0)
		return Duration();

	return Duration(data.it_interval.tv_sec * 1000 + data.it_interval.tv_nsec / 1000000);
}


bool WindowSystemPrivate::isHandleValid(Widget::Handle handle) const
{
	xcb_generic_error_t* error;
	auto cookie = xcb_get_window_attributes(connection_, handle);
	auto reply = xcb_get_window_attributes_reply(connection_, cookie, &error);

	if(!reply) {
		LOG("invalid handle: {0}", (int) error->error_code);
		return false;
	}

	free(reply);
	return true;
}


void WindowSystemPrivate::processEvents()
{
	epoll_event event;
	bool running = true;

	while(running) {
		int count = epoll_wait(epollFd_, &event, 1, -1);
		if(count < 0 && errno != EINTR) {
			LOG("epoll_wait call failed: {0}", ::strerror(errno));
			break;
		}

		int fd = event.data.fd;
		if(fd == xcbFd_) {
			processWindowEvents();
		}
		else if(fd == pipeFds_[0]) {
			Command command;
			read(fd, &command, sizeof(command));

			if(command == Command::kStopProcessing) {
				while(!deletionQueue_.empty()) {
					auto it = deletionQueue_.begin();
					delete (*it);
					deletionQueue_.erase(it);
				}

				running = false;
			}
			else if(command == Command::kRepaintWidgets) {
				while(!repaintQueue_.empty()) {
					auto it = repaintQueue_.begin();
					(*it)->repaint();
					repaintQueue_.erase(it);
				}
			}
			else if(command == Command::kDeleteWidgets) {
				while(!deletionQueue_.empty()) {
					auto it = deletionQueue_.begin();
					delete (*it);
					deletionQueue_.erase(it);
				}
			}
		}
		else {
			u64 value;
			read(fd, &value, sizeof(value));

			const Timer* timer = timerByHandle(fd);
			if(timer)
				timer->timeout({});
		}
	}
}


void WindowSystemPrivate::stopEventProcessing()
{
	Command command = Command::kStopProcessing;
	write(pipeFds_[1], &command, sizeof(command));
}


void WindowSystemPrivate::processWindowEvents()
{
	while(xcb_generic_event_t* event = xcb_poll_for_event(connection_)) {
		switch(event->response_type & ~0x80) {
		case XCB_MAP_NOTIFY: {
			auto ev = reinterpret_cast<xcb_map_notify_event_t*>(event);

			const WindowData* data = dataByHandle(ev->window);
			if(data) {
				Event e(Event::kShow);
				data->widget->processEvent(&e, {});
			}

			break; }

		case XCB_UNMAP_NOTIFY: {
			auto ev = reinterpret_cast<xcb_unmap_notify_event_t*>(event);

			const WindowData* data = dataByHandle(ev->window);
			if(data) {
				Event e(Event::kHide);
				data->widget->processEvent(&e, {});
			}

			break; }

		case XCB_EXPOSE: {
			auto ev = reinterpret_cast<xcb_expose_event_t*>(event);

			const WindowData* data = dataByHandle(ev->window);
			if(data) {
				Painter painter(data->surface, {});

				PaintEvent e;
				e.setPainter(&painter, {});
				e.setRect(Rect<int>(ev->x, ev->y, ev->width, ev->height), {});
				data->widget->processEvent(&e, {});
			}

			xcb_flush(connection_);
			break; }

		case XCB_CONFIGURE_NOTIFY: {
			if(!(event->response_type & 0x80))
				break;

			auto ev = reinterpret_cast<xcb_configure_notify_event_t*>(event);

			const WindowData* data = dataByHandle(ev->window);
			if(data) {
				Size<int> size(ev->width, ev->height);
				if(size != data->widget->size()) {
					cairo_xcb_surface_set_size(data->surface, ev->width, ev->height);

					ResizeEvent e;
					e.setSize(size, {});
					e.setOldSize(data->widget->size(), {});
					data->widget->processEvent(&e, {});

					data->widget->repaint();
				}

				Point<int> pos(ev->x, ev->y);
				if(pos != data->widget->pos()) {
					MoveEvent e;
					e.setPos(pos, {});
					e.setOldPos(data->widget->pos(), {});
					data->widget->processEvent(&e, {});
				}
			}

			break; }

		case XCB_FOCUS_IN: {
			auto ev = reinterpret_cast<xcb_focus_in_event_t*>(event);

			const WindowData* data = dataByHandle(ev->event);
			if(data) {
				Event e(Event::kFocusIn);
				data->widget->processEvent(&e, {});
			}

			break; }

		case XCB_FOCUS_OUT: {
			auto ev = reinterpret_cast<xcb_focus_out_event_t*>(event);

			const WindowData* data = dataByHandle(ev->event);
			if(data) {
				Event e(Event::kFocusOut);
				data->widget->processEvent(&e, {});
			}

			break; }

		case XCB_ENTER_NOTIFY: {
			auto ev = reinterpret_cast<xcb_enter_notify_event_t*>(event);

			const WindowData* data = dataByHandle(ev->event);
			if(data) {
				Point<int> pos(ev->event_x, ev->event_y);
				Point<int> globalPos(ev->root_x, ev->root_y);

				MouseEvent e(Event::kMouseEnter);
				e.setTimestamp(ev->time, {});
				e.setModifiers(translateKeyModifier(ev->state), {});
				e.setPos(pos, {});
				e.setWindowPos(pos, {});
				e.setGlobalPos(globalPos, {});
				e.setButtons(mouseButtons_, {});
				data->widget->processEvent(&e, {});
			}
			break; }

		case XCB_LEAVE_NOTIFY: {
			auto ev = reinterpret_cast<xcb_leave_notify_event_t*>(event);

			const WindowData* data = dataByHandle(ev->event);
			if(data) {
				Point<int> pos(ev->event_x, ev->event_y);
				Point<int> globalPos(ev->root_x, ev->root_y);

				MouseEvent e(Event::kMouseLeave);
				e.setTimestamp(ev->time, {});
				e.setModifiers(translateKeyModifier(ev->state), {});
				e.setPos(pos, {});
				e.setWindowPos(pos, {});
				e.setGlobalPos(globalPos, {});
				e.setButtons(mouseButtons_, {});
				data->widget->processEvent(&e, {});
			}

			break; }

		case XCB_MOTION_NOTIFY: {
			auto ev = reinterpret_cast<xcb_motion_notify_event_t*>(event);

			const WindowData* data = dataByHandle(ev->event);
			if(data) {
				Point<int> pos(ev->event_x, ev->event_y);
				Point<int> globalPos(ev->root_x, ev->root_y);

				MouseEvent e(Event::kMouseMove);
				e.setTimestamp(ev->time, {});
				e.setModifiers(translateKeyModifier(ev->state), {});
				e.setPos(pos, {});
				e.setWindowPos(pos, {});
				e.setGlobalPos(globalPos, {});
				e.setButtons(mouseButtons_, {});
				data->widget->processEvent(&e, {});
			}

			break; }

		case XCB_BUTTON_PRESS: {
			auto ev = reinterpret_cast<xcb_button_press_event_t*>(event);

			const WindowData* data = dataByHandle(ev->event);
			if(data) {
				Point<int> pos(ev->event_x, ev->event_y);
				Point<int> globalPos(ev->root_x, ev->root_y);

				bool isVertical;
				int delta;
				if(checkWheelEvent(ev->detail, &isVertical, &delta)) {
					MouseWheelEvent e;
					e.setTimestamp(ev->time, {});
					e.setModifiers(translateKeyModifier(ev->state), {});
					e.setPos(pos, {});
					e.setWindowPos(pos, {});
					e.setGlobalPos(globalPos, {});
					e.setVertical(isVertical, {});
					e.setDelta(delta, {});
					e.setButtons(mouseButtons_, {});
					data->widget->processEvent(&e, {});
					break;
				}

				MouseButton button = translateMouseButton(ev->detail);
				mouseButtons_ ^= button;

				if(button != MouseButton::kNone) {
					MouseEvent e(Event::kMousePress);
					e.setTimestamp(ev->time, {});
					e.setModifiers(translateKeyModifier(ev->state), {});
					e.setPos(pos, {});
					e.setWindowPos(pos, {});
					e.setGlobalPos(globalPos, {});
					e.setButton(button, {});
					e.setButtons(mouseButtons_, {});
					data->widget->processEvent(&e, {});
				}
			}

			break; }

		case XCB_BUTTON_RELEASE: {
			auto ev = reinterpret_cast<xcb_button_release_event_t*>(event);

			const WindowData* data = dataByHandle(ev->event);
			if(data) {
				Point<int> pos(ev->event_x, ev->event_y);
				Point<int> globalPos(ev->root_x, ev->root_y);

				MouseButton button = translateMouseButton(ev->detail);
				mouseButtons_ &= ~Flags<MouseButton>(button);

				if(button != MouseButton::kNone) {
					MouseEvent e(Event::kMouseRelease);
					e.setTimestamp(ev->time, {});
					e.setModifiers(translateKeyModifier(ev->state), {});
					e.setPos(pos, {});
					e.setWindowPos(pos, {});
					e.setGlobalPos(globalPos, {});
					e.setButton(button, {});
					e.setButtons(mouseButtons_, {});
					data->widget->processEvent(&e, {});
				}
			}

			break; }

		case XCB_KEY_PRESS: {
			auto ev = reinterpret_cast<xcb_key_press_event_t*>(event);

			const WindowData* data = dataByHandle(ev->event);
			if(data) {
				KeyEvent e(Event::kKeyPress);
				e.setTimestamp(ev->time, {});
				e.setModifiers(translateKeyModifier(ev->state), {});
				e.setKey(keycodeToKey(ev->detail), {});
				data->widget->processEvent(&e, {});
			}

			break; }

		case XCB_KEY_RELEASE: {
			auto ev = reinterpret_cast<xcb_key_release_event_t*>(event);

			const WindowData* data = dataByHandle(ev->event);
			if(data) {
				KeyEvent e(Event::kKeyRelease);
				e.setTimestamp(ev->time, {});
				e.setModifiers(translateKeyModifier(ev->state), {});
				e.setKey(keycodeToKey(ev->detail), {});
				data->widget->processEvent(&e, {});
			}

			break; }

		case XCB_CLIENT_MESSAGE: {
			auto ev = reinterpret_cast<xcb_client_message_event_t*>(event);

			const WindowData* data = dataByHandle(ev->window);
			if(ev->data.data32[0] == data->wmDeleteWindow) {
				Event e(Event::kClose);
				data->widget->processEvent(&e, {});
			}

			break; }

		default:
			break;
		}

		free(event);
	}
}


const WindowSystemPrivate::WindowData*
WindowSystemPrivate::dataByHandle(Widget::Handle handle) const
{
	auto it = dataByHandle_.find(handle);
	if(it == dataByHandle_.end())
		return nullptr;

	return &it->second;
}


Timer* WindowSystemPrivate::timerByHandle(Timer::Handle handle)
{
	auto it = timerByHandle_.find(handle);
	if(it == timerByHandle_.end())
		return nullptr;

	return it->second;
}


MouseButton WindowSystemPrivate::translateMouseButton(u8 button)
{
	switch(button) {
	case 1:
		return MouseButton::kLeft;

	case 2:
		return MouseButton::kMiddle;

	case 3:
		return MouseButton::kRight;

	default:
		return MouseButton::kNone;
	}
}


KeyModifiers WindowSystemPrivate::translateKeyModifier(u16 state)
{
	KeyModifiers result;

	if(state & XCB_MOD_MASK_SHIFT)
		result |= KeyModifier::kShift;

	if(state & XCB_MOD_MASK_CONTROL)
		result |= KeyModifier::kControl;

	if(state & XCB_MOD_MASK_1)
		result |= KeyModifier::kAlt;

	if(state & XCB_MOD_MASK_4)
		result |= KeyModifier::kMeta;

	return result;
}


bool WindowSystemPrivate::checkWheelEvent(u8 detail, bool* vertical, int* delta)
{
	if(detail == 4) {
		*vertical = true;
		*delta = 120;
		return true;
	}
	else if(detail == 5) {
		*vertical = true;
		*delta = -120;
		return true;
	}
	else if(detail == 6) {
		*vertical = false;
		*delta = 120;
		return true;
	}
	else if(detail == 7) {
		*vertical = false;
		*delta = -120;
		return true;
	}

	return false;
}


Key WindowSystemPrivate::keycodeToKey(int keysym)
{
	static const Key kKeyTable[256] = {
			Key::kNoKey
	};

	return kKeyTable[keysym];
}


} // namespace Tech
