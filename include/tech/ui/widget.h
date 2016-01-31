#ifndef TECH_UI_WIDGET_H
#define TECH_UI_WIDGET_H

#include <tech/flags.h>
#include <tech/passkey.h>
#include <tech/signal.h>
#include <tech/string.h>
#include <tech/timecounter.h>
#include <tech/types.h>
#include <tech/ui/events.h>
#include <tech/ui/rect.h>


using cairo_surface_t = struct _cairo_surface;


namespace Tech {


class Event;
class Layout;
class MoveEvent;
class ResizeEvent;
class PaintEvent;
class MouseEvent;
class MouseWheelEvent;
class Painter;
class WindowSystem;
class WindowSystemPrivate;


enum class WindowFlag {
	kNone             = 0x00,
	kFrameless        = 0x01,
	kSkipTaskbar      = 0x02,
	kNoShadow         = 0x04,
	kFullscreen       = 0x08,
	kNoCloseButton    = 0x10,
	kNoMinimizeButton = 0x20,
	kNoMaximizeButton = 0x40,
	kNoHelpButton     = 0x80,
	kStaysOnTop       = 0x100
};

using WindowFlags = Flags<WindowFlag>;
DECLARE_FLAG_OPERATORS(WindowFlag)


class Widget : public Trackable {
public:
	using Handle = uint;
	static const Handle kInvalidHandle = 0;

	Widget(Widget* parent = nullptr, WindowFlags flags = WindowFlag::kNone);

	Widget(const Point<int>& pos, const Size<int>& size, Widget* parent = nullptr,
			WindowFlags flags = WindowFlag::kNone);

	Widget(int x, int y, int width, int height, Widget* parent = nullptr,
			WindowFlags flags = WindowFlag::kNone);

	Widget(int x, int y, int width, int height, Widget::Handle owner);

	Widget(const Widget& other) = delete;

	~Widget() override;

	Widget& operator=(const Widget& other) = delete;

	Handle handle() const;

	WindowFlags windowFlags() const;
	void setWindowFlags(WindowFlags flags);

	bool isEmbedded() const;

	bool isEnabled() const;
	void setEnabled(bool enable);
	void toggleEnabled();
	void enable();
	void disable();

	bool isVisible() const;
	void setVisible(bool visible);
	void toggleVisible();
	void show();
	void hide();

	int x() const;
	int y() const;
	int width() const;
	int height() const;
	Point<int> pos() const;
	Size<int> size() const;
	Rect<int> rect() const;

	Rect<int> geometry() const;
	void setGeometry(const Rect<int>& geometry);
	void setGeometry(int x, int y, int width, int height);

	void move(int x, int y);
	void move(const Point<int>& pos);

	void resize(int width, int height);
	void resize(const Size<int>& size);

	Widget* parent() const;
	void setParent(Widget* parent, const Point<int>& pos = Point<int>());
	void setParent(Widget* parent, int x, int y);

	Widget* childAt(const Point<int>& point, bool recursive = true) const;

	Point<int> mapFrom(const Widget* parent, const Point<int>& point) const;
	Point<int> mapFromGlobal(const Point<int>& point = Point<int>()) const;
	Point<int> mapFromWindow(const Point<int>& point = Point<int>()) const;
	Point<int> mapFromParent(const Point<int>& point = Point<int>()) const;

	Point<int> mapTo(const Widget* parent, const Point<int>& point) const;
	Point<int> mapToGlobal(const Point<int>& point = Point<int>()) const;
	Point<int> mapToWindow(const Point<int>& point = Point<int>()) const;
	Point<int> mapToParent(const Point<int>& point = Point<int>()) const;

	void update();
	void repaint();

	void deleteLater();

	bool hasFocus() const;
	bool isUnderMouse() const;

	String windowTitle() const;
	void setWindowTitle(const String& title);

	/**
	 * Returns a perfect size of the widget. This size is used by layout manager of the
	 * parent widget (if it's present). Negative size hint values are treated as a
	 * "stretchable size". The layout manager of parent widget can set any size between
	 * minimumSize() and maximumSize() to widgets, whose sizeHint() is "stretchable". If
	 * you reimplement this function, you should notify your parent widget on every
	 * change of size hint's value by calling parent()->handleChildSizeHintChange(this).
	 */
	virtual Size<int> sizeHint() const;

	Size<int> minimumSize() const;
	void setMinimumSize(int width, int height);
	void setMinimumSize(const Size<int>& size);

	int minimumWidth() const;
	void setMinimumWidth(int width);

	int minimumHeight() const;
	void setMinimumHeight(int height);

	Size<int> maximumSize() const;
	void setMaximumSize(int width, int height);
	void setMaximumSize(const Size<int>& size);

	int maximumWidth() const;
	void setMaximumWidth(int width);

	int maximumHeight() const;
	void setMaximumHeight(int height);

	void setFixedSize(int width, int height);
	void setFixedSize(const Size<int>& size);
	void setFixedWidth(int width);
	void setFixedHeight(int height);

	Layout* layout() const;
	void setLayout(Box<Layout> layout);

	bool isProcessingEvents() const;
	void processEvents();
	void stopEventProcessing();

restricted:
	void processEvent(Event* event, PassKey<Widget, WindowSystemPrivate>);

protected:
	virtual bool dispatchEvent(Event* event);

	virtual void handleShow(Event* event);
	virtual void handleHide(Event* event);

	virtual void handleEnable(Event* event);
	virtual void handleDisable(Event* event);

	virtual void handleClose(Event* event);

	virtual void handlePaint(PaintEvent* event);

	virtual void handleMove(MoveEvent* event);
	virtual void handleResize(ResizeEvent* event);

	virtual void handleFocusIn(Event* event);
	virtual void handleFocusOut(Event* event);

	virtual void handleMouseEnter(Event* event);
	virtual void handleMouseLeave(MouseEvent* event);

	virtual void handleMouseMove(MouseEvent* event);

	virtual void handleMousePress(MouseEvent* event);
	virtual void handleMouseRelease(MouseEvent* event);

	virtual void handleMouseDoubleClick(MouseEvent* event);
	virtual void handleMouseWheel(MouseWheelEvent* event);

	virtual void handleKeyPress(KeyEvent* event);
	virtual void handleKeyRelease(KeyEvent* event);

	/**
	 * This function is called right after the size hint of the child widget has changed.
	 * Default implementation notifies the layout manager about the event.
	 * If you reimplement this function and don't want to break the layout manager
	 * functionality, then you should call the base version.
	 */
	virtual void handleChildSizeHintChange(Widget* child);

private:
	// Window handle. Widgets without a parent have an invalid handle (kInvalidHandle).
	Handle handle_;

	// Window surface. For all child widgets the surface_ is pointed to the farthest
	// parent's (window) surface.
	cairo_surface_t* surface_;

	WindowFlags windowFlags_;

	struct {
		// Indicates that widget is embedded into foreign window
		bool isEmbedded_         : 1;

		// Indicates that widget itself is enabled, though it can still be disabled
		// because of a disabled parent
		bool isEnabled_          : 1;

		// Indicates that widget inherits the disabled state from its parent
		bool hasDisabledParent_  : 1;

		// Indicates that widget itself is visible, though it can still be invisible
		// because of an invisible parent
		bool isVisible_          : 1;

		// Indicates that widget inherits invisibility from its parent
		bool hasHiddenParent_    : 1;

		// Indicates that widget accepts the keyboard input focus
		bool isAcceptsFocus_     : 1;

		// Indicates that widget has the keyboard input focus
		bool hasFocus_           : 1;

		// Indicates that hover_ will receive all mouse events until at least one mouse
		// button is pressed
		bool hasGrab_            : 1;

		// Indicates that processEvents() function of this widget is running
		bool isProcessingEvents_ : 1;
	};

	// Position of widget relative to parent's origin
	Point<int> pos_;

	// Size of widget
	Size<int> size_;

	// Parent widget
	Widget* parent_;

	// Previous sibling widget (higher sibling)
	Widget* previous_;

	// Next sibling widget (lower sibling)
	Widget* next_;

	// First child widget (topmost child)
	Widget* first_;

	// Last child widget (lowest child)
	Widget* last_;

	// Child widget which contains a mouse pointer or nullptr
	Widget* hover_;

	// If widget is a window, then this field points to a child widget which has a
	// keyboard input focus
	Widget* focus_;

	// Counts the time between mouse button release events. Used for generation of
	// double click events.
	TimeCounter timeCounter_;

	// Mouse button, that was used in last mouse click on the widget. Used for
	// generation of double click events.
	MouseButton lastButton_;

	// Window title if widget is a window
	String title_;

	// A minimum size of the widget, which can be set by layout manager
	Size<int> minimumSize_;

	// A maximum size of the widget, which can be set by layout manager
	Size<int> maximumSize_;

	Box<Layout> layout_;

	void applyWindowFlags(WindowFlags flags);

	void recursiveShow();
	void recursiveHide();

	void recursiveEnable();
	void recursiveDisable();

	void sendMouseLeave();

	Tuple<Widget*, Point<int>> hoveredWidget();
	Tuple<Widget*, Point<int>> updateHoverPoint(const Point<int>& point);

	Widget* processMouseEvent(MouseEvent* event);
	void updateFocus(Widget* window);

	void repaint(const Rect<int>& rect);
	void repaint(Painter* painter, const Rect<int>& rect, const Point<int>& pos);
};


} // namespace Tech


#endif // TECH_UI_WIDGET_H
