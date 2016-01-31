#ifndef TECH_UI_GRADIENT_H
#define TECH_UI_GRADIENT_H

#include <tech/ui/point.h>


using cairo_pattern_t = struct _cairo_pattern;


namespace Tech {


class Color;


class Gradient {
public:
	Gradient(const Gradient& other);
	virtual ~Gradient();

	Gradient& operator=(const Gradient& other);

	void setColorAt(double offset, const Color& color);

protected:
	Gradient(cairo_pattern_t* pattern);

private:
	friend class Painter;
	cairo_pattern_t* pattern_;
};


class LinearGradient final : public Gradient {
public:
	LinearGradient(double x0, double y0, double x1, double y1);
	LinearGradient(const Point<double>& p0, const Point<double>& p1);
};


class RadialGradient final : public Gradient {
public:
	RadialGradient(double cx0, double cy0, double radius0, double cx1, double cy1,
			double radius1);

	RadialGradient(const Point<double>& p0, double radius0,	const Point<double>& p1,
			double radius1);
};


} // namespace Tech


#endif // TECH_UI_GRADIENT_H
