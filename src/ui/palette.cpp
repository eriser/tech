#include <tech/ui/palette.h>

#define WINDOW_ROLE			  0
#define WINDOW_TEXT_ROLE	  1
#define BASE_ROLE			  2
#define TEXT_ROLE			  3
#define HIGHLIGHT_ROLE		  4
#define HIGHLIGHTED_TEXT_ROLE 5
#define TOOLTIP_ROLE	      6
#define TOOLTIP_TEXT_ROLE	  7
#define BUTTON_ROLE			  8
#define BUTTON_TEXT_ROLE	  9


namespace Tech {


Palette::Palette()
{
	colors_[WINDOW_ROLE][kActive]      	      = Color(0.7, 0.7, 0.7);
	colors_[WINDOW_TEXT_ROLE][kActive] 	      = Color(0.0, 0.0, 0.0);
	colors_[BASE_ROLE][kActive]        	      = Color(1.0, 1.0, 1.0);
	colors_[TEXT_ROLE][kActive]               = Color(0.0, 0.0, 0.0);
	colors_[HIGHLIGHT_ROLE][kActive]          = Color(0.0, 0.0, 0.6);
	colors_[HIGHLIGHTED_TEXT_ROLE][kActive]   = Color(0.0, 0.0, 0.0);
	colors_[TOOLTIP_ROLE][kActive]            = Color(0.7, 0.7, 0.0);
	colors_[TOOLTIP_TEXT_ROLE][kActive]       = Color(0.0, 0.0, 0.0);
	colors_[BUTTON_ROLE][kActive]             = Color(0.7, 0.7, 0.7);
	colors_[BUTTON_TEXT_ROLE][kActive]        = Color(0.0, 0.0, 0.0);

	colors_[WINDOW_ROLE][kInactive]           = Color(0.7, 0.7, 0.7);
	colors_[WINDOW_TEXT_ROLE][kInactive]      = Color(0.0, 0.0, 0.0);
	colors_[BASE_ROLE][kInactive]             = Color(1.0, 1.0, 1.0);
	colors_[TEXT_ROLE][kInactive]             = Color(0.0, 0.0, 0.0);
	colors_[HIGHLIGHT_ROLE][kInactive]        = Color(0.0, 0.0, 0.6);
	colors_[HIGHLIGHTED_TEXT_ROLE][kInactive] = Color(0.0, 0.0, 0.0);
	colors_[TOOLTIP_ROLE][kInactive]          = Color(0.7, 0.7, 0.0);
	colors_[TOOLTIP_TEXT_ROLE][kInactive]     = Color(0.0, 0.0, 0.0);
	colors_[BUTTON_ROLE][kInactive]           = Color(0.7, 0.7, 0.7);
	colors_[BUTTON_TEXT_ROLE][kInactive]      = Color(0.0, 0.0, 0.0);

	colors_[WINDOW_ROLE][kDisabled]           = Color(0.7, 0.7, 0.7);
	colors_[WINDOW_TEXT_ROLE][kDisabled]      = Color(0.5, 0.5, 0.5);
	colors_[BASE_ROLE][kDisabled]             = Color(0.9, 0.9, 0.9);
	colors_[TEXT_ROLE][kDisabled]             = Color(0.5, 0.5, 0.5);
	colors_[HIGHLIGHT_ROLE][kDisabled]        = Color(0.0, 0.0, 0.6);
	colors_[HIGHLIGHTED_TEXT_ROLE][kDisabled] = Color(0.0, 0.0, 0.0);
	colors_[TOOLTIP_ROLE][kDisabled]          = Color(0.7, 0.7, 0.0);
	colors_[TOOLTIP_TEXT_ROLE][kDisabled]     = Color(0.5, 0.5, 0.5);
	colors_[BUTTON_ROLE][kDisabled]           = Color(0.8, 0.8, 0.8);
	colors_[BUTTON_TEXT_ROLE][kDisabled]      = Color(0.5, 0.5, 0.5);
}


Color Palette::window(Palette::State state) const
{
	return colors_[WINDOW_ROLE][state];
}


void Palette::setWindow(const Color& color, Palette::State state)
{
	colors_[WINDOW_ROLE][state] = color;
}


Color Palette::windowText(Palette::State state) const
{
	return colors_[WINDOW_TEXT_ROLE][state];
}


void Palette::setWindowText(const Color& color, Palette::State state)
{
	colors_[WINDOW_TEXT_ROLE][state] = color;
}


Color Palette::base(Palette::State state) const
{
	return colors_[BASE_ROLE][state];
}


void Palette::setBase(const Color& color, Palette::State state)
{
	colors_[BASE_ROLE][state] = color;
}


Color Palette::text(Palette::State state) const
{
	return colors_[TEXT_ROLE][state];
}


void Palette::setText(const Color& color, Palette::State state)
{
	colors_[TEXT_ROLE][state] = color;
}


Color Palette::highlight(Palette::State state) const
{
	return colors_[HIGHLIGHT_ROLE][state];
}


void Palette::setHighlight(const Color& color, Palette::State state)
{
	colors_[HIGHLIGHT_ROLE][state] = color;
}


Color Palette::highlightedText(Palette::State state) const
{
	return colors_[HIGHLIGHTED_TEXT_ROLE][state];
}


void Palette::setHighlightedText(const Color& color, Palette::State state)
{
	colors_[HIGHLIGHTED_TEXT_ROLE][state] = color;
}


Color Palette::toolTip(Palette::State state) const
{
	return colors_[TOOLTIP_ROLE][state];
}


void Palette::setToolTip(const Color& color, Palette::State state)
{
	colors_[TOOLTIP_ROLE][state] = color;
}


Color Palette::toolTipText(Palette::State state) const
{
	return colors_[TOOLTIP_TEXT_ROLE][state];
}


void Palette::setToolTipText(const Color& color, Palette::State state)
{
	colors_[TOOLTIP_TEXT_ROLE][state] = color;
}


Color Palette::button(Palette::State state) const
{
	return colors_[BUTTON_ROLE][state];
}


void Palette::setButton(const Color& color, Palette::State state)
{
	colors_[BUTTON_ROLE][state] = color;
}


Color Palette::buttonText(Palette::State state) const
{
	return colors_[BUTTON_TEXT_ROLE][state];
}


void Palette::setButtonText(const Color& color, Palette::State state)
{
	colors_[BUTTON_TEXT_ROLE][state] = color;
}


} // namespace Tech
