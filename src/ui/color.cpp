#include <tech/ui/color.h>

#include <limits>
#include <tech/utils.h>


namespace Tech {


Color::Color() :
	alpha_(0.0f),
	red_(0.0f),
	green_(0.0f),
	blue_(0.0f)
{
}


Color::Color(double red, double green, double blue, double alpha) :
	alpha_(alpha),
	red_(red),
	green_(green),
	blue_(blue)
{
}


Color::Color(u32 argb) :
	alpha_(((argb >> 24) & 0xFF) / 255.0),
	red_  (((argb >> 16) & 0xFF) / 255.0),
	green_(((argb >> 8)  & 0xFF) / 255.0),
	blue_ ((argb         & 0xFF) / 255.0)
{
}


double Color::alpha() const
{
	return alpha_;
}


void Color::setAlpha(double value)
{
	alpha_ = bound(0.0, value, 1.0);
}


double Color::red() const
{
	return red_;
}


void Color::setRed(double value)
{
	red_ = bound(0.0, value, 1.0);
}


double Color::green() const
{
	return green_;
}


void Color::setGreen(double value)
{
	green_ = bound(0.0, value, 1.0);
}


double Color::blue() const
{
	return blue_;
}


void Color::setBlue(double value)
{
	blue_ = bound(0.0, value, 1.0);
}


double Color::hue() const
{
	double minValue = minOf(red_, green_, blue_);
	double maxValue = maxOf(red_, green_, blue_);

	if(maxValue == 0.0)
		return std::numeric_limits<double>::quiet_NaN();

	double delta = maxValue - minValue;
	double result;

	if(red_ >= maxValue) {
		result = (green_ - blue_) / delta;
	}
	else if(green_ >= maxValue) {
		result = 2.0 + (blue_ - red_) / delta;
	}
	else {
		result = 4.0 + (red_ - green_) / delta;
	}

	return result * (1.0 / 6.0);
}


void Color::setHue(double value)
{

}


double Color::saturation() const
{
	double minValue = minOf(red_, green_, blue_);
	double maxValue = maxOf(red_, green_, blue_);
	double delta = maxValue - minValue;

	if(maxValue > 0.0)
		return delta / maxValue;

	return 0.0;
}


void Color::setSaturation(double value)
{
}


double Color::value() const
{
	return maxOf(red_, green_, blue_);
}


void Color::setValue(double value)
{
}


Color Color::fromArgb8888(u8 red, u8 green, u8 blue, u8 alpha)
{
	return Color(static_cast<u32>(alpha) << 24 | static_cast<u32>(red) << 16
			| static_cast<u32>(green) << 8 | blue);
}


} // namespace Tech
