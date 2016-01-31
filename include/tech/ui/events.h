#ifndef TECH_UI_EVENTS_H
#define TECH_UI_EVENTS_H

#include <tech/flags.h>
#include <tech/passkey.h>
#include <tech/types.h>
#include <tech/ui/keys.h>
#include <tech/ui/rect.h>


namespace Tech {


class Widget;
class WindowSystemPrivate;


class Event {
public:
	enum Type : u8 {
		kShow,
		kHide,
		kEnable,
		kDisable,
		kClose,
		kMove,
		kResize,
		kPaint,
		kFocusIn,
		kFocusOut,
		kMouseEnter,
		kMouseLeave,
		kMouseMove,
		kMousePress,
		kMouseRelease,
		kMouseDoubleClick,
		kMouseWheel,
		kKeyPress,
		kKeyRelease
	};

	Event(Type type) :
		type_(type),
		isAccepted_(true)
	{
	}

	Type type() const
	{
		return type_;
	}

	bool isAccepted() const
	{
		return isAccepted_;
	}

	void accept()
	{
		isAccepted_ = true;
	}

	void ignore()
	{
		isAccepted_ = false;
	}

	void setAccepted(bool accepted)
	{
		isAccepted_ = accepted;
	}

private:
	Type type_;
	bool isAccepted_;
};


class MoveEvent : public Event {
public:
	MoveEvent() :
		Event(kMove)
	{
	}

	Point<int> pos() const
	{
		return pos_;
	}

	void setPos(const Point<int>& pos, PassKey<Widget, WindowSystemPrivate>)
	{
		pos_ = pos;
	}

	Point<int> oldPos() const
	{
		return oldPos_;
	}

	void setOldPos(const Point<int>& oldPos, PassKey<Widget, WindowSystemPrivate>)
	{
		oldPos_ = oldPos;
	}

private:
	Point<int> pos_;
	Point<int> oldPos_;
};


class ResizeEvent : public Event {
public:
	ResizeEvent() :
		Event(kResize)
	{
	}

	Size<int> size() const
	{
		return size_;
	}

	void setSize(const Size<int>& size, PassKey<Widget, WindowSystemPrivate>)
	{
		size_ = size;
	}

	Size<int> oldSize() const
	{
		return oldSize_;
	}

	void setOldSize(const Size<int>& oldSize, PassKey<Widget, WindowSystemPrivate>)
	{
		oldSize_ = oldSize;
	}

private:
	Size<int> size_;
	Size<int> oldSize_;
};


class Painter;


class PaintEvent : public Event {
public:
	PaintEvent() :
		Event(kPaint),
		painter_(nullptr)
	{
	}

	Rect<int> rect() const
	{
		return rect_;
	}

	void setRect(const Rect<int>& rect, PassKey<Widget, WindowSystemPrivate>)
	{
		rect_ = rect;
	}

	Painter* painter() const
	{
		return painter_;
	}

	void setPainter(Painter* painter, PassKey<Widget, WindowSystemPrivate>)
	{
		painter_ = painter;
	}

private:
	Rect<int> rect_;
	Painter* painter_;
};


class InputEvent : public Event {
public:
	InputEvent(Type type) :
		Event(type),
		timestamp_(0)
	{
	}

	ulong timestamp() const
	{
		return timestamp_;
	}

	void setTimestamp(ulong timestamp, PassKey<Widget, WindowSystemPrivate>)
	{
		timestamp_ = timestamp;
	}

	KeyModifiers modifiers() const
	{
		return modifiers_;
	}

	void setModifiers(KeyModifiers modifiers, PassKey<Widget, WindowSystemPrivate>)
	{
		modifiers_ = modifiers;
	}

private:
	ulong timestamp_;
	KeyModifiers modifiers_;
};


enum class MouseButton {
	kNone   = 0x00,
	kLeft   = 0x01,
	kRight  = 0x02,
	kMiddle = 0x03
};

using MouseButtons = Flags<MouseButton>;


class MouseEvent : public InputEvent {
public:
	MouseEvent(Type type) :
		InputEvent(type)
	{
	}

	Point<int> pos() const
	{
		return pos_;
	}

	void setPos(const Point<int>& pos, PassKey<Widget, WindowSystemPrivate>)
	{
		pos_ = pos;
	}

	Point<int> windowPos() const
	{
		return windowPos_;
	}

	void setWindowPos(const Point<int>& pos, PassKey<Widget, WindowSystemPrivate>)
	{
		windowPos_ = pos;
	}

	Point<int> globalPos() const
	{
		return globalPos_;
	}

	void setGlobalPos(const Point<int>& pos, PassKey<Widget, WindowSystemPrivate>)
	{
		globalPos_ = pos;
	}

	MouseButton button() const
	{
		return button_;
	}

	void setButton(MouseButton button, PassKey<Widget, WindowSystemPrivate>)
	{
		button_ = button;
	}

	MouseButtons buttons() const
	{
		return buttons_;
	}

	void setButtons(MouseButtons buttons, PassKey<Widget, WindowSystemPrivate>)
	{
		buttons_ = buttons;
	}

private:
	Point<int> pos_;
	Point<int> windowPos_;
	Point<int> globalPos_;
	MouseButton button_;
	MouseButtons buttons_;
};


class MouseWheelEvent : public MouseEvent {
public:
	MouseWheelEvent() :
		MouseEvent(kMouseWheel),
		isVertical_(true),
		delta_(0)
	{
	}

	bool isVertical() const
	{
		return isVertical_;
	}

	void setVertical(bool vertical, PassKey<Widget, WindowSystemPrivate>)
	{
		isVertical_ = vertical;
	}

	int delta() const
	{
		return delta_;
	}

	void setDelta(int delta, PassKey<Widget, WindowSystemPrivate>)
	{
		delta_ = delta;
	}

private:
	bool isVertical_;
	int delta_;
};


class KeyEvent : public InputEvent {
public:
	KeyEvent(Type type) :
		InputEvent(type),
		key_(Key::kNoKey),
		isAutoRepeat_(false)
	{
	}

	Key key() const
	{
		return key_;
	}

	void setKey(Key key, PassKey<Widget, WindowSystemPrivate>)
	{
		key_ = key;
	}

	bool isAutoRepeat() const
	{
		return isAutoRepeat_;
	}

	void setAutoRepeat(bool value, PassKey<Widget, WindowSystemPrivate>)
	{
		isAutoRepeat_ = value;
	}

private:
	Key key_;
	bool isAutoRepeat_;
};


} // namespace Tech


#endif // TECH_UI_EVENTS_H
