#include <tech/ui/widget.h>

#include <tech/logger.h>
#include <tech/ui/layout.h>
#include <tech/ui/painter.h>
#include "windowsystem.h"


namespace Tech {


using namespace Literals;


Widget::Widget(Widget* parent, WindowFlags flags) :
	Widget(Point<int>(), Size<int>(100, 100), parent, flags)
{
}


Widget::Widget(const Point<int>& pos, const Size<int>& size, Widget* parent,
		WindowFlags flags) :
	ws_(WindowSystem::instance()),
	handle_(kInvalidHandle),
	windowFlags_(flags),
	isInitialized_(false),
	isEnabled_(true),
	hasDisabledParent_(false),
	isVisible_(false),
	hasHiddenParent_(false),
	isAcceptsFocus_(true),
	hasFocus_(false),
	hasGrab_(false),
	isProcessingEvents_(false),
	embedder_(kInvalidHandle),
	pos_(pos),
	size_(size),
	parent_(nullptr),
	previous_(nullptr),
	next_(nullptr),
	first_(nullptr),
	last_(nullptr),
	hover_(nullptr),
	focus_(nullptr),
	maximumSize_(Limits<int>::max(), Limits<int>::max())
{
	setParent(parent, pos_);
	isInitialized_ = true;
}


Widget::Widget(int x, int y, int width, int height, Widget* parent, WindowFlags flags) :
	Widget(Point<int>(x, y), Size<int>(width, height), parent, flags)
{
}


Widget::Widget(int x, int y, int width, int height, Widget::Handle embedder) :
	ws_(WindowSystem::instance()),
	handle_(kInvalidHandle),
	windowFlags_(WindowFlag::kFrameless),
	isEnabled_(true),
	hasDisabledParent_(false),
	isVisible_(false),
	hasHiddenParent_(false),
	isAcceptsFocus_(true),
	hasFocus_(false),
	hasGrab_(false),
	isProcessingEvents_(false),
	pos_(Point<int>(x, y)),
	size_(Size<int>(width, height)),
	parent_(nullptr),
	previous_(nullptr),
	next_(nullptr),
	first_(nullptr),
	last_(nullptr),
	hover_(nullptr),
	focus_(nullptr),
	maximumSize_(Limits<int>::max(), Limits<int>::max())
{
	setParent(embedder, x, y);
	isInitialized_ = true;
}


Widget::~Widget()
{
	Widget* child = first_;

	while(child) {
		Widget* next = child->next_;
		delete child;
		child = next;
	}

	if(parent_) {
		if(parent_->layout_)
			parent_->layout_->remove(this);
	}
	else {
		ws_->destroyWindow(handle_);
		ws_->sync();
	}
}


Widget::Handle Widget::handle() const
{
	return handle_;
}


WindowFlags Widget::windowFlags() const
{
	return windowFlags_;
}


void Widget::setWindowFlags(WindowFlags flags)
{
	applyWindowFlags(flags);
	windowFlags_ = flags;
}


void Widget::applyWindowFlags(WindowFlags flags)
{
	if(flags & WindowFlag::kNoShadow) {
		ws_->setWindowFrameless(handle_, true);
	}
	else {
		ws_->setWindowFrameless(handle_, false);
	}

	if(flags & WindowFlag::kSkipTaskbar) {
		ws_->setWindowTaskbarButton(handle_, true);
	}
	else {
		ws_->setWindowTaskbarButton(handle_, false);
	}

	if(flags & WindowFlag::kNoShadow) {

	}
	else {

	}

	if(flags & WindowFlag::kFullscreen) {

	}
	else {

	}
	if(flags & WindowFlag::kNoCloseButton) {

	}
	else {

	}
	if(flags & WindowFlag::kNoMinimizeButton) {

	}
	else {

	}
	if(flags & WindowFlag::kNoMaximizeButton) {

	}
	else {

	}
	if(flags & WindowFlag::kNoHelpButton) {

	}
	else {

	}
	if(flags & WindowFlag::kStaysOnTop) {

	}
	else {

	}
}


bool Widget::isEmbedded() const
{
	return embedder_ != kInvalidHandle;
}


bool Widget::isEnabled() const
{
	return isEnabled_ && !hasDisabledParent_;
}


void Widget::setEnabled(bool enable)
{
	if(isEnabled_ != enable) {
		isEnabled_ = enable;

		if(hasDisabledParent_)
			return;

		if(enable) {
			recursiveEnable();
		}
		else {
			recursiveDisable();
		}

		repaint();
	}
}


void Widget::toggleEnabled()
{
	setEnabled(!isEnabled_);
}


void Widget::enable()
{
	setEnabled(true);
}


void Widget::disable()
{
	setEnabled(false);
}


void Widget::recursiveEnable()
{
	Event event(Event::kEnable);
	dispatchEvent(&event);

	Widget* widget = first_;

	while(widget) {
		widget->hasDisabledParent_ = false;

		if(widget->isEnabled_)
			widget->recursiveEnable();

		widget = widget->next_;
	}
}


void Widget::recursiveDisable()
{
	if(hasFocus_) {
		hasFocus_ = false;

		Event event(Event::kFocusOut);
		dispatchEvent(&event);

		// TODO update window->focus_
		// TODO update focus receiver
	}

	Event event(Event::kDisable);
	dispatchEvent(&event);

	Widget* widget = first_;

	while(widget) {
		widget->hasDisabledParent_ = true;

		if(widget->isEnabled_)
			widget->recursiveDisable();

		widget = widget->next_;
	}
}


bool Widget::isVisible() const
{
	return isVisible_ && !hasHiddenParent_;
}


void Widget::setVisible(bool visible)
{
	if(isVisible_ != visible) {
		isVisible_ = visible;

		if(hasHiddenParent_)
			return;

		if(visible) {
			recursiveShow();
		}
		else {
			recursiveHide();
		}

		if(!parent_) {
			ws_->setWindowVisible(handle_, visible);
			ws_->sync();
		}
		else {
			repaint();
		}
	}
}


void Widget::toggleVisible()
{
	setVisible(!isVisible_);
}


void Widget::show()
{
	setVisible(true);
}


void Widget::hide()
{
	setVisible(false);
}


void Widget::recursiveShow()
{
	Event event(Event::kShow);
	dispatchEvent(&event);

	Widget* widget = first_;

	while(widget) {
		widget->hasHiddenParent_ = false;

		if(widget->isVisible_)
			widget->recursiveShow();

		widget = widget->next_;
	}
}


void Widget::recursiveHide()
{
	if(hasFocus_) {
		hasFocus_ = false;

		Event event(Event::kFocusOut);
		dispatchEvent(&event);

		// TODO update window->focus_
		// TODO update focus receiver
	}

	Event event(Event::kHide);
	dispatchEvent(&event);

	Widget* widget = first_;

	while(widget) {
		widget->hasHiddenParent_ = true;

		if(widget->isVisible_)
			widget->recursiveHide();

		widget = widget->next_;
	}
}


int Widget::x() const
{
	return pos_.x();
}


int Widget::y() const
{
	return pos_.y();
}


int Widget::width() const
{
	return size_.width();
}


int Widget::height() const
{
	return size_.height();
}


Point<int> Widget::pos() const
{
	return pos_;
}


Size<int> Widget::size() const
{
	return size_;
}


Rect<int> Widget::rect() const
{
	return Rect<int>(Point<int>(), size_);
}


Rect<int> Widget::geometry() const
{
	return Rect<int>(pos_, size_);;
}


void Widget::setGeometry(const Rect<int>& geometry)
{
	if(!geometry.size().isValid())
		return;

	if(!parent_) {
		ws_->moveWindow(handle_, geometry.topLeft());
		ws_->resizeWindow(handle_, geometry.size());
		ws_->sync();
	}
	else {
		// Calculate invalidated rectangle
		int x = minOf(pos_.x(), geometry.x());
		int y = minOf(pos_.y(), geometry.y());
		int width = maxOf(size_.width(), geometry.size().width());
		int height = maxOf(size_.height(), geometry.size().height());
		Rect<int> rect(x, y, width, height);

		// Send events
		MoveEvent me;
		me.setOldPos(pos_, {});
		me.setPos(geometry.topLeft(), {});
		processEvent(&me, {});

		ResizeEvent re;
		re.setOldSize(size_, {});
		re.setSize(geometry.size(), {});
		processEvent(&re, {});

		rect.intersect(parent_->rect());
		parent_->repaint();
	}
}


void Widget::setGeometry(int x, int y, int width, int height)
{
	setGeometry(Rect<int>(x, y, width, height));
}


void Widget::move(int x, int y)
{
	move(Point<int>(x, y));
}


void Widget::move(const Point<int>& pos)
{
	if(pos_ == pos)
		return;

	if(!parent_) {
		ws_->moveWindow(handle_, pos);
		ws_->sync();
	}
	else {
		// Calculate invalidated rectangle
		Rect<int> rect = geometry();
		Point<int> diff = pos - pos_;

		if(diff.x() > 0) {
			rect.setWidth(rect.width() + diff.x());
		}
		else if(diff.x() < 0) {
			rect.setX(rect.x() + diff.x());
			rect.setWidth(rect.width() - diff.x());
		}

		if(diff.y() > 0) {
			rect.setHeight(rect.height() + diff.y());
		}
		else if(diff.y() < 0) {
			rect.setY(rect.y() + diff.y());
			rect.setHeight(rect.height() - diff.y());
		}

		// Send event
		MoveEvent e;
		e.setOldPos(pos_, {});
		e.setPos(pos, {});
		processEvent(&e, {});

		rect.intersect(parent_->rect());
		parent_->repaint();
	}
}


void Widget::resize(int width, int height)
{
	resize(Size<int>(width, height));
}


void Widget::resize(const Size<int>& size)
{
	if(!size.isValid() || size_ == size)
		return;

	int width = bound(minimumSize_.width(), size.width(), maximumSize_.width());
	int height = bound(minimumSize_.height(), size.height(), maximumSize_.height());

	if(!parent_) {
		ws_->resizeWindow(handle_, Size<int>(width, height));
		ws_->sync();
	}
	else {
		// Calculate invalidated rectangle
		int x = pos_.x();
		int y = pos_.y();
		width = maxOf(size_.width(), width);
		height = maxOf(size_.height(), height);
		Rect<int> rect(x, y, width, height);

		// Send event
		ResizeEvent e;
		e.setOldSize(size_, {});
		e.setSize(size, {});
		processEvent(&e, {});

		rect.intersect(parent_->rect());
		parent_->repaint();
	}
}


Widget* Widget::parent() const
{
	return parent_;
}


void Widget::setParent(Widget* parent, const Point<int>& pos)
{
	setParent(parent, pos, kInvalidHandle);
}


void Widget::setParent(Widget* parent, int x, int y)
{
	setParent(parent, Point<int>(x, y), kInvalidHandle);
}


void Widget::setParent(Handle embedder, const Point<int>& pos)
{
	setParent(nullptr, pos, embedder);
}


void Widget::setParent(Handle embedder, int x, int y)
{
	setParent(nullptr, Point<int>(x, y), embedder);
}


void Widget::setParent(Widget* newParent, const Point<int>& pos, Handle embedder)
{
	if(isInitialized_ && newParent == parent_ && embedder == embedder_)
		return;

	// Widget should become invisible when its parent changed
	hide();

	if(parent_) {
		// Extract widget from its parent

		if(parent_->first_ == this)
			parent_->first_ = next_;

		if(parent_->last_ == this)
			parent_->last_ = previous_;

		if(next_)
			next_->previous_ = previous_;

		if(previous_)
			previous_->next_ = next_;

		if(layout_)
			layout_->remove(this);
	}
	else if(handle_ != kInvalidHandle) {
		ws_->destroyWindow(handle_);
		handle_ = kInvalidHandle;
	}

	parent_ = newParent;
	previous_ = nullptr;

	if(newParent) { // Insert widget into the new parent
		next_ = newParent->first_;

		if(newParent->first_)
			newParent->first_->previous_ = this;

		newParent->first_ = this;

		if(!newParent->last_)
			newParent->last_ = this;

		pos_ = pos;

		hasHiddenParent_ = !newParent->isVisible();

		if(newParent->isEnabled()) {
			if(hasDisabledParent_) {
				hasDisabledParent_ = false;
				if(isEnabled_)
					recursiveEnable();
			}
		}
		else if(!hasDisabledParent_) {
			if(isEnabled_)
				recursiveDisable();
		}

		if(layout_)
			layout_->insert(this);
	}
	else { // Create window for widget without the parent widget
		next_ = nullptr;
		handle_ = ws_->createWindow(this, embedder);
		if(handle_ == kInvalidHandle) {
			LOG("Unable to create native window");
			return;
		}

		if(embedder == kInvalidHandle) {
			ws_->moveWindow(handle_, pos_);
			ws_->resizeWindow(handle_, size_);

			// Remove the WindowFlag::kFrameless flag
			WindowFlags flags = ~windowFlags_;
			flags &= windowFlags_;

			applyWindowFlags(flags);
		}

		hasHiddenParent_ = false;

		if(hasDisabledParent_) {
			hasDisabledParent_ = false;
			if(isEnabled_)
				recursiveEnable();
		}
	}

	embedder_ = embedder;
	ws_->sync();
}


Widget* Widget::childAt(const Point<int>& point, bool recursive) const
{
	Widget* child = first_;

	while(child) {
		if(child->rect().contains(point)) {
			if(recursive)
				return child->childAt(point - child->pos_);

			return child;
		}

		child = child->next_;
	}

	return nullptr;
}


Point<int> Widget::mapFrom(const Widget* parent, const Point<int>& point) const
{
	const Widget* widget = this;
	Point<int> result = point;

	while(widget != parent) {
		result -= widget->pos_;
		widget = widget->parent_;
	}

	return result;
}


Point<int> Widget::mapFromGlobal(const Point<int>& point) const
{
//	TODO:
//	if(isEmbedded_)
//		 return WINDOW_SYSTEM->mapFromGlobal(handle_, point);

	return mapFrom(nullptr, point);
}


Point<int> Widget::mapFromWindow(const Point<int>& point) const
{
	const Widget* widget = this;
	Point<int> result = point;

	while(widget->parent_) {
		result -= widget->pos_;
		widget = widget->parent_;
	}

	return result;
}


Point<int> Widget::mapFromParent(const Point<int>& point) const
{
	return point - pos_;
}


Point<int> Widget::mapTo(const Widget* parent, const Point<int>& point) const
{
	const Widget* widget = this;
	Point<int> result = point;

	while(widget != parent) {
		result += widget->pos_;
		widget = widget->parent_;
	}

	return result;
}


Point<int> Widget::mapToGlobal(const Point<int>& point) const
{
//	TODO:
//	if(isEmbedded_)
//		 return WINDOW_SYSTEM->mapToGlobal(handle_, point);

	return mapTo(nullptr, point);
}


Point<int> Widget::mapToWindow(const Point<int>& point) const
{
	const Widget* widget = this;
	Point<int> result = point;

	while(widget->parent_) {
		result += widget->pos_;
		widget = widget->parent_;
	}

	return result;
}


Point<int> Widget::mapToParent(const Point<int>& point) const
{
	return point + pos_;
}


void Widget::update()
{
	ws_->enqueueWidgetRepaint(this);
}


void Widget::repaint()
{
	if(!isVisible())
		return;

	Widget* widget = this;
	Rect<int> r = rect();

	// Step down to the lowest widget (window)
	while(widget->parent_) {
		r.translate(widget->pos_);
		widget = widget->parent_;
	}

	ws_->repaintWindow(widget->handle_, r);
	ws_->sync();
}


void Widget::repaint(PaintEvent* event)
{
	Painter* painter = event->painter();
	painter->beginOffscreenPaint();

	repaintWidget(event);

	painter->applyOffscreenPaint();
}


void Widget::repaintWidget(PaintEvent* event)
{
	Painter* painter = event->painter();
	painter->save();

	if(parent_)
		painter->translate(pos().x(), pos().y());

	painter->rectangle(event->rect());
	painter->clip();

	if(!parent_) {
		painter->setSource(Color::kWhite);
		painter->paint();
	}

	PaintEvent e(*event);
	dispatchEvent(&e);

	painter->restore();

	// Repaint child widgets from bottom to top
	Widget* widget = last_;

	while(widget) {
		Rect<int> r = event->rect().intersected(widget->geometry());
		r.translate(-widget->pos());
		e.setRect(r, {});
		widget->repaintWidget(&e);

		widget = widget->previous_;
	}
}


void Widget::deleteLater()
{
	ws_->enqueueWidgetDeletion(this);
}


bool Widget::hasFocus() const
{
	return hasFocus_;
}


bool Widget::isUnderMouse() const
{
	return !parent_ || parent_->hover_ == this;
}


String Widget::windowTitle() const
{
	return title_;
}


void Widget::setWindowTitle(const String& title)
{
	if(handle_ != kInvalidHandle) {
		ws_->setWindowTitle(handle_, title);
		ws_->sync();
	}

	title_ = title;
}


Size<int> Widget::sizeHint() const
{
	return Size<int>(-1, -1);
}


Size<int> Widget::minimumSize() const
{
	return minimumSize_;
}


void Widget::setMinimumSize(int width, int height)
{
	setMinimumSize(Size<int>(width, height));
}


void Widget::setMinimumSize(const Size<int>& size)
{
	minimumSize_ = size;

	if(!parent_) {
		ws_->setWindowSizeLimits(handle_, minimumSize_, maximumSize_);
		ws_->sync();
	}
}


int Widget::minimumWidth() const
{
	return minimumSize_.width();
}


void Widget::setMinimumWidth(int width)
{
	minimumSize_.setWidth(width);

	if(!parent_) {
		ws_->setWindowSizeLimits(handle_, minimumSize_, maximumSize_);
		ws_->sync();
	}
}


int Widget::minimumHeight() const
{
	return minimumSize_.height();
}


void Widget::setMinimumHeight(int height)
{
	minimumSize_.setHeight(height);

	if(!parent_) {
		ws_->setWindowSizeLimits(handle_, minimumSize_, maximumSize_);
		ws_->sync();
	}
}


Size<int> Widget::maximumSize() const
{
	return maximumSize_;
}


void Widget::setMaximumSize(int width, int height)
{
	setMaximumSize(Size<int>(width, height));
}


void Widget::setMaximumSize(const Size<int>& size)
{
	maximumSize_ = size;

	if(!parent_) {
		ws_->setWindowSizeLimits(handle_, minimumSize_, maximumSize_);
		ws_->sync();
	}
}


int Widget::maximumWidth() const
{
	return maximumSize_.width();
}


void Widget::setMaximumWidth(int width)
{
	maximumSize_.setWidth(width);

	if(!parent_) {
		ws_->setWindowSizeLimits(handle_, minimumSize_, maximumSize_);
		ws_->sync();
	}
}


int Widget::maximumHeight() const
{
	return maximumSize_.height();
}


void Widget::setMaximumHeight(int height)
{
	maximumSize_.setHeight(height);

	if(!parent_) {
		ws_->setWindowSizeLimits(handle_, minimumSize_, maximumSize_);
		ws_->sync();
	}
}


void Widget::setFixedSize(int width, int height)
{
	setFixedSize(Size<int>(width, height));
}


void Widget::setFixedSize(const Size<int>& size)
{
	minimumSize_ = size;
	maximumSize_ = size;

	if(!parent_) {
		ws_->setWindowSizeLimits(handle_, minimumSize_, maximumSize_);
		ws_->sync();
	}
}


void Widget::setFixedWidth(int width)
{
	minimumSize_.setWidth(width);
	maximumSize_.setWidth(width);

	if(!parent_) {
		ws_->setWindowSizeLimits(handle_, minimumSize_, maximumSize_);
		ws_->sync();
	}
}


void Widget::setFixedHeight(int height)
{
	minimumSize_.setHeight(height);
	maximumSize_.setHeight(height);

	if(!parent_) {
		ws_->setWindowSizeLimits(handle_, minimumSize_, maximumSize_);
		ws_->sync();
	}
}


Layout* Widget::layout() const
{
	return layout_.get();
}


void Widget::setLayout(Box<Layout> layout)
{
	layout_ = std::move(layout);
	layout_->setGeometry(rect());
}


bool Widget::isProcessingEvents() const
{
	return isProcessingEvents_;
}


void Widget::processEvents()
{
	isProcessingEvents_ = true;
	ws_->processEvents();
}


void Widget::stopEventProcessing()
{
	if(isProcessingEvents_) {
		isProcessingEvents_ = false;
		ws_->stopEventProcessing();
	}
}


void Widget::processEvent(Event* event, PassKey<Widget, WindowSystemPrivate>)
{
	switch(event->type()) {
	case Event::kFocusIn: {
		if(isAcceptsFocus_) {
			Widget* widget = first_;
			Widget* next = widget;

			while(next) {
				widget = next;

				if(widget->isAcceptsFocus_ && widget->first_ && widget->isVisible()
				   && widget->isEnabled()) {
					next = widget->first_;
				}
				else {
					next = next->next_;
				}
			}

			if(focus_)
				focus_->hasFocus_ = false;

			focus_ = widget;

			if(focus_) {
				focus_->hasFocus_ = true;
				focus_->dispatchEvent(event);
			}
		}

		return; }

	case Event::kFocusOut:
		if(focus_) {
			focus_->hasFocus_ = false;
			focus_->dispatchEvent(event);
		}

		return;

	case Event::kMouseEnter: {
		if(!hasGrab_) {
			MouseEvent* e = static_cast<MouseEvent*>(event);
			dispatchEvent(e);

			updateHoverPoint(e->pos());
		}

		return; }

	case Event::kMouseLeave: {
		if(!hasGrab_)
			sendMouseLeave();

		return; }

	case Event::kMouseMove: {
		MouseEvent* e = static_cast<MouseEvent*>(event);
		Widget* widget;
		Point<int> pos;

		if(hasGrab_) {
			std::tie(widget, pos) = hoveredWidget();

			e->setPos(e->windowPos() - pos, {});
			widget->processMouseEvent(e);
		}
		else {
			std::tie(widget, pos) = updateHoverPoint(e->pos());

			e->setPos(pos, {});
			widget->dispatchEvent(e);
		}

		return; }

	case Event::kMousePress: {
		MouseEvent* e = static_cast<MouseEvent*>(event);
		hasGrab_ = true;

		Widget* widget;
		Point<int> pos;
		std::tie(widget, pos) = hoveredWidget();

		widget->updateFocus(this);

		e->setPos(e->windowPos() - pos, {});
		widget->processMouseEvent(e);
		return; }

	case Event::kMouseRelease: {
		MouseEvent* e = static_cast<MouseEvent*>(event);

		Widget* widget;
		Point<int> pos;
		std::tie(widget, pos) = hoveredWidget();

		Point<int> eventPos = e->pos();
		e->setPos(e->windowPos() - pos, {});
		widget->processMouseEvent(e);

		if(e->buttons() == MouseButton::kNone) {
			hasGrab_ = false;
			updateHoverPoint(eventPos);
		}

		Duration duration = widget->timeCounter_.restart();
		if(duration <= 300_ms && e->button() != MouseButton::kNone
		   && e->button() == widget->lastButton_) {
			MouseEvent ev(Event::kMouseDoubleClick);
			ev.setTimestamp(e->timestamp(), {});
			ev.setModifiers(e->modifiers(), {});
			ev.setPos(e->pos(), {});
			ev.setWindowPos(e->windowPos(), {});
			ev.setGlobalPos(e->globalPos(), {});
			ev.setButton(e->button(), {});
			ev.setButtons(e->buttons(), {});
			widget->processEvent(&ev, {});

			widget->lastButton_ = MouseButton::kNone;
		}
		else {
			widget->lastButton_ = e->button();
		}

		return; }

	case Event::kMouseWheel: {
		MouseWheelEvent* e = static_cast<MouseWheelEvent*>(event);

		Widget* widget;
		Point<int> pos;
		std::tie(widget, pos) = hoveredWidget();

		e->setPos(e->windowPos() - pos, {});
		widget->processMouseEvent(e);
		return; }

	case Event::kKeyPress: {
		Widget* widget = focus_;
		while(focus_) {
			if(focus_->dispatchEvent(event)) {
				if(event->isAccepted())
					return;
			}

			widget = widget->parent_;
		}

		return; }

	case Event::kKeyRelease: {
		Widget* widget = focus_;
		while(focus_) {
			if(focus_->dispatchEvent(event)) {
				if(event->isAccepted())
					return;
			}

			widget = widget->parent_;
		}

		return; }

	case Event::kShow: {
		show();
		break; }

	case Event::kHide: {
		hide();
		break; }

	case Event::kEnable: {
		enable();
		break; }

	case Event::kDisable: {
		disable();
		break; }

	case Event::kClose : {
		if(dispatchEvent(event) && event->isAccepted()) {
			hide();
		}

		return; }

	case Event::kPaint: {
		repaint(static_cast<PaintEvent*>(event));

		// repaint() calls the dispatchEvent() by itself so we don't need to call it one
		// more time
		return; }

	case Event::kMove: {
		MoveEvent* e = static_cast<MoveEvent*>(event);
		pos_ = e->pos();
		break; }

	case Event::kResize: {
		ResizeEvent* e = static_cast<ResizeEvent*>(event);
		size_ = e->size();
		break; }

	default:
		break;
	}

	dispatchEvent(event);

	if(event->type() == Event::kResize && layout_) {
		layout_->setGeometry(rect());
	}
}


bool Widget::dispatchEvent(Event* event)
{
	switch(event->type()) {
	case Event::kShow:
		handleShow(event);
		break;

	case Event::kHide:
		handleHide(event);
		break;

	case Event::kEnable:
		handleEnable(event);
		break;

	case Event::kDisable:
		handleDisable(event);
		break;

	case Event::kClose:
		handleClose(event);
		break;

	case Event::kPaint:
		handlePaint(static_cast<PaintEvent*>(event));
		break;

	case Event::kMove:
		handleMove(static_cast<MoveEvent*>(event));
		break;

	case Event::kResize:
		handleResize(static_cast<ResizeEvent*>(event));
		break;

	case Event::kFocusIn:
		handleFocusIn(event);
		break;

	case Event::kFocusOut:
		handleFocusOut(event);
		break;

	case Event::kMouseEnter:
		handleMouseEnter(static_cast<MouseEvent*>(event));
		break;

	case Event::kMouseLeave:
		handleMouseLeave(static_cast<MouseEvent*>(event));
		break;

	case Event::kMouseMove:
		handleMouseMove(static_cast<MouseEvent*>(event));
		break;

	case Event::kMousePress:
		handleMousePress(static_cast<MouseEvent*>(event));
		break;

	case Event::kMouseRelease:
		handleMouseRelease(static_cast<MouseEvent*>(event));
		break;

	case Event::kMouseDoubleClick:
		handleMouseDoubleClick(static_cast<MouseEvent*>(event));
		break;

	case Event::kMouseWheel:
		handleMouseWheel(static_cast<MouseWheelEvent*>(event));
		break;

	case Event::kKeyPress:
		handleKeyPress(static_cast<KeyEvent*>(event));
		break;

	case Event::kKeyRelease:
		handleKeyRelease(static_cast<KeyEvent*>(event));
		break;

	default:
		break;
	}

	return true;
}


void Widget::sendMouseLeave()
{
	Widget* widget = this;

	while(widget->hover_)
		widget = widget->hover_;

	widget->lastButton_ = MouseButton::kNone;
	widget->hover_ = nullptr;

	Event event(Event::kMouseLeave);

	while(widget != this->parent_) {
		if(widget->parent_)
			widget->parent_->hover_ = nullptr;

		widget->dispatchEvent(&event);
		widget = widget->parent_;
	}
}


Tuple<Widget*, Point<int>> Widget::hoveredWidget()
{
	Widget* widget = this;
	Point<int> point;

	while(widget->hover_) {
		widget = widget->hover_;
		point += widget->pos_;
	}

	return makeTuple(widget, point);
}


Tuple<Widget*, Point<int>> Widget::updateHoverPoint(const Point<int>& point)
{
	Widget* widget = this;
	Widget* result = this;
	Point<int> pos = point;
	MouseEvent event(Event::kMouseEnter);

	while(widget) {
		Point<int> origin;
		if(widget->parent_)
			origin = widget->pos_;

		Rect<int> rect(origin, widget->size_);

		if(widget->isVisible() && widget->isEnabled() && rect.contains(pos)) {
			pos -= origin;

			if(widget->parent_ && widget->parent_->hover_ != widget) {
				if(widget->parent_->hover_)
					widget->parent_->hover_->sendMouseLeave();

				widget->parent_->hover_ = widget;

				event.setPos(pos, {});
				widget->dispatchEvent(&event);
			}

			result = widget;
			widget = widget->first_;
		}
		else {
			widget = widget->next_;
		}
	}

	if(result->hover_) {
		Widget* oldHover = result->hover_;
		result->hover_ = nullptr;
		oldHover->sendMouseLeave();
	}

	return makeTuple(result, pos);
}


Widget* Widget::processMouseEvent(MouseEvent* event)
{
	Widget* widget = this;

	while(widget) {
		widget->dispatchEvent(event);
		if(event->isAccepted())
			break;

		event->setPos(event->pos() + widget->pos_, {});
		widget = widget->parent_;
	}

	return widget;
}


void Widget::updateFocus(Widget* window)
{
	Widget* widget = this;
	Widget* oldFocus = window->focus_;

	while(widget && !widget->isAcceptsFocus_) {
		widget = widget->parent_;
	}

	Widget* next = widget;

	while(next && next->isEnabled() && next->isVisible() && next->isAcceptsFocus_) {
		widget = next;

		if(widget->first_) {
			next = widget->first_;
		}
		else {
			break;
		}
	}

	if(oldFocus != widget) {
		window->focus_ = widget;

		if(oldFocus) {
			Event e(Event::kFocusOut);
			oldFocus->dispatchEvent(&e);
		}

		if(widget) {
			Event e(Event::kFocusIn);
			window->focus_->dispatchEvent(&e);
		}
	}
}


void Widget::handleShow(Event* event)
{
	UNUSED(event);
}


void Widget::handleHide(Event* event)
{
	UNUSED(event);
}


void Widget::handleEnable(Event* event)
{
	UNUSED(event);
}


void Widget::handleDisable(Event* event)
{
	UNUSED(event);
}


void Widget::handleClose(Event* event)
{
	UNUSED(event);
}


void Widget::handlePaint(PaintEvent* event)
{
	UNUSED(event);
}


void Widget::handleMove(MoveEvent* event)
{
	UNUSED(event);
}


void Widget::handleResize(ResizeEvent* event)
{
	UNUSED(event);
}


void Widget::handleFocusIn(Event* event)
{
	UNUSED(event);
}


void Widget::handleFocusOut(Event* event)
{
	UNUSED(event);
}


void Widget::handleMouseEnter(Event* event)
{
	UNUSED(event);
}


void Widget::handleMouseLeave(MouseEvent* event)
{
	UNUSED(event);
}


void Widget::handleMouseMove(MouseEvent* event)
{
	UNUSED(event);
}


void Widget::handleMousePress(MouseEvent* event)
{
	UNUSED(event);
}


void Widget::handleMouseRelease(MouseEvent* event)
{
	UNUSED(event);
}


void Widget::handleMouseDoubleClick(MouseEvent* event)
{
	UNUSED(event);
}


void Widget::handleMouseWheel(MouseWheelEvent* event)
{
	UNUSED(event);
}


void Widget::handleKeyPress(KeyEvent* event)
{
	UNUSED(event);
}


void Widget::handleKeyRelease(KeyEvent* event)
{
	UNUSED(event);
}


void Widget::handleChildSizeHintChange(Widget* child)
{
	UNUSED(child);

	if(layout_)
		layout_->update();
}


} // namespace Tech
