#ifndef TECH_UI_BUTTON_H
#define TECH_UI_BUTTON_H

#include <tech/ui/widget.h>


namespace Tech {


class Button : public Widget {
public:
	Button(const String& text, Widget* parent = nullptr);

signals:
	Signal<void(), PassKey<Button>> clicked;

protected:
	void handleMouseEnter(Event* event) override;
	void handleMouseLeave(MouseEvent* event) override;
	void handleMousePress(MouseEvent* event) override;
	void handleMouseRelease(MouseEvent* event) override;
	void handlePaint(PaintEvent* event) override;
	void handleKeyPress(KeyEvent* event) override;
	void handleKeyRelease(KeyEvent* event) override;

private:
	String text_;
	bool isPressed_;
};


} // namespace Tech


#endif // TECH_UI_BUTTON_H
