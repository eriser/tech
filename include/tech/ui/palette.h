#ifndef TECH_PORTS_PALETTE_H
#define TECH_PORTS_PALETTE_H

#include <tech/ui/color.h>


namespace Tech {


class Palette final {
public:
	enum State {
		kActive,
		kInactive,
		kDisabled
	};

	Palette();

	Color window(State state = kActive) const;
	void setWindow(const Color& color, State state = kActive);

	Color windowText(State state = kActive) const;
	void setWindowText(const Color& color, State state = kActive);

	Color base(State state = kActive) const;
	void setBase(const Color& color, State state = kActive);

	Color text(State state = kActive) const;
	void setText(const Color& color, State state = kActive);

	Color highlight(State state = kActive) const;
	void setHighlight(const Color& color, State state = kActive);

	Color highlightedText(State state = kActive) const;
	void setHighlightedText(const Color& color, State state = kActive);

	Color toolTip(State state = kActive) const;
	void setToolTip(const Color& color, State state = kActive);

	Color toolTipText(State state = kActive) const;
	void setToolTipText(const Color& color, State state = kActive);

	Color button(State state = kActive) const;
	void setButton(const Color& color, State state = kActive);

	Color buttonText(State state = kActive) const;
	void setButtonText(const Color& color, State state = kActive);

private:
	Color colors_[10][3];
};


} // namespace Tech


#endif // TECH_PORTS_PALETTE_H
