#include <tech/ui/button.h>

#include <tech/logger.h>
#include <tech/ui/color.h>
#include <tech/ui/fontmetrics.h>
#include <tech/ui/painter.h>


namespace Tech {


Button::Button(const String& text, Widget* parent) :
    Widget(0, 0, 80, 28, parent),
	text_(text),
	isPressed_(false)
{
}


void Button::handleMouseEnter(Event* event)
{
	UNUSED(event);
//	LOG("mouse enter: (%p)", this);
	update();
}


void Button::handleMouseLeave(MouseEvent* event)
{
	UNUSED(event);
//	LOG("mouse leave: (%p)", this);
	update();
}


void Button::handleMousePress(MouseEvent* event)
{
//	LOG("mouse press: %d:%d (%p)", event->pos().x(), event->pos().y(), this);

	if(event->button() == MouseButton::kLeft) {
		isPressed_ = true;
		update();
	}
}


void Button::handleMouseRelease(MouseEvent* event)
{
//	LOG("mouse release: %d:%d (%p)", event->pos().x(), event->pos().y(), this);

	if(event->button() == MouseButton::kLeft) {
		isPressed_ = false;
		update();
		clicked({});
	}
}


void Button::handleKeyPress(KeyEvent* event)
{
//	if(event->key() == 36) {
		isPressed_ = true;
		update();
//	}
}


void Button::handleKeyRelease(KeyEvent* event)
{
//	if(event->key() == 36) {
		isPressed_ = false;
		update();
//	}
}

void Button::handlePaint(PaintEvent* event)
{
    Painter* painter = event->painter();
    painter->setLineWidth(1);

    double r = 8;
	double x = 1.0;
	double y = 1.0;
	double w = width() - 2;
	double h = height() - 2;

    painter->moveTo(x + r, y);
	painter->lineTo(x + w - r, y);
	painter->curveTo(x + w, y, x + w, y, x + w, y + r);
	painter->lineTo(x + w, y + h - r);
	painter->curveTo(x + w, y + h, x + w, y + h, x + w - r, y + h);
	painter->lineTo(x + r, y + h);
	painter->curveTo(x, y + h, x, y + h, x, y + h - r);
	painter->lineTo(x, y + r);
	painter->curveTo(x, y, x, y, x + r, y);

    if(isPressed_) {
		painter->setSource(Color::kMagenta);
        painter->fill(true);
        painter->setSource(Color::kDarkMagenta);
    }
	else if(isUnderMouse()) {
        painter->setSource(Color(0.0, 1.0, 0.0));
        painter->fill(true);
        painter->setSource(Color(0.0, 0.4, 0.0));
    }
	else {
        painter->setSource(Color(0.0, 0.93, 0.0));
        painter->fill(true);
        painter->setSource(Color(0.0, 0.5, 0.0));
    }

    painter->stroke();

	FontMetrics fm(painter->font());
	Size<int> size = fm.size(text_);

	x = width() / 2 - size.width() / 2;
	y = height() / 2 - size.height() / 2;
    painter->drawText(x, y, text_);
}


} // namespace Tech
