#ifndef TECH_UI_COLOR_H
#define TECH_UI_COLOR_H

#include <tech/types.h>


namespace Tech {


class Color final {
public:
	enum ColorName {
		kTransparent = 0x00000000,
		kBlack       = 0xFF000000,
		kRed         = 0xFFFF0000,
		kGreen       = 0xFF00FF00,
		kBlue        = 0xFF0000FF,
		kYellow      = 0xFFFFFF00,
		kCyan        = 0xFF00FFFF,
		kMagenta     = 0xFFFF00FF,
		kWhite       = 0xFFFFFFFF,
		kDarkRed     = 0xFF800000,
		kDarkGreen   = 0xFF008000,
		kDarkBlue    = 0xFF000080,
		kDarkYellow  = 0xFF808000,
		kDarkCyan    = 0xFF008080,
		kDarkMagenta = 0xFF800080,
		kGray        = 0xFFA0A0A0,
		kDarkGray    = 0xFF808080,
		kLightGray   = 0xFFC0C0C0
	};

	Color();
	Color(double red, double green, double blue, double alpha = 1.0);
	Color(u32 argb);

	double alpha() const;
	void setAlpha(double value);

	double red() const;
	void setRed(double value);

	double green() const;
	void setGreen(double value);

	double blue() const;
	void setBlue(double value);

	double hue() const;
	void setHue(double value);

	double saturation() const;
	void setSaturation(double value);

	double value() const;
	void setValue(double value);

	static Color fromArgb8888(u8 red, u8 green, u8 blue, u8 alpha);

private:
	friend class Painter;

	double alpha_;
	double red_;
	double green_;
	double blue_;
};


} // namespace Tech


#endif // TECH_UI_COLOR_H
