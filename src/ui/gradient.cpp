#include <tech/ui/gradient.h>

#include <cairo.h>
#include <tech/utils.h>
#include <tech/ui/color.h>


namespace Tech {


Gradient::Gradient(cairo_pattern_t* pattern) :
	pattern_(pattern)
{
}


Gradient::Gradient(const Gradient& other)
{
	*this = other;
}


Gradient::~Gradient()
{
	cairo_pattern_destroy(pattern_);
}


Gradient& Gradient::operator=(const Gradient& other)
{
	cairo_pattern_type_t type = cairo_pattern_get_type(other.pattern_);

	if(type == CAIRO_PATTERN_TYPE_LINEAR) {
		double x0, y0, x1, y1;
		cairo_pattern_get_linear_points(other.pattern_, &x0, &y0, &x1, &y1);
		pattern_ = cairo_pattern_create_linear(x0, y0, x1, y1);
	}
	else {
		double x0, y0, r0, x1, y1, r1;
		cairo_pattern_get_radial_circles(other.pattern_, &x0, &y0, &r0, &x1, &y1, &r1);
		pattern_ = cairo_pattern_create_radial(x0, y0, r0, x1, y1, r1);
	}

	double offset, r, b, g, a;
	int count;
	cairo_pattern_get_color_stop_count(other.pattern_, &count);

	for(int i = 0; i < count; ++i) {
		cairo_pattern_get_color_stop_rgba(other.pattern_, i, &offset, &r, &g, &b, &a);
		cairo_pattern_add_color_stop_rgba(pattern_, offset, r, g, b, a);
	}

	return *this;
}


void Gradient::setColorAt(double offset, const Color& color)
{
	offset = bound(0.0, offset, 1.0);

	cairo_pattern_add_color_stop_rgba(pattern_, offset, color.red(), color.green(),
			color.blue(), color.alpha());
}


LinearGradient::LinearGradient(double x0, double y0, double x1, double y1) :
	Gradient(cairo_pattern_create_linear(x0, y0, x1, y1))
{
}


LinearGradient::LinearGradient(const Point<double>& p0, const Point<double>& p1) :
	Gradient(cairo_pattern_create_linear(p0.x(), p0.y(), p1.x(), p1.y()))
{
}


RadialGradient::RadialGradient(double cx0, double cy0, double radius0, double cx1,
		double cy1, double radius1) :
	Gradient(cairo_pattern_create_radial(cx0, cy0, radius0, cx1, cy1, radius1))
{
}


RadialGradient::RadialGradient(const Point<double>& p0, double r0,
		const Point<double>& p1, double r1) :
	Gradient(cairo_pattern_create_radial(p0.x(), p0.y(), r0, p1.x(), p1.y(), r1))
{
}


} // namespace Tech
