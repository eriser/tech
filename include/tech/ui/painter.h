#ifndef TECH_UI_PAINTER_H
#define TECH_UI_PAINTER_H

#include <stack>
#include <tech/passkey.h>
#include <tech/string.h>
#include <tech/ui/color.h>
#include <tech/ui/font.h>
#include <tech/ui/line.h>
#include <tech/ui/rect.h>


using cairo_t = struct _cairo;
using cairo_surface_t = struct _cairo_surface;
using cairo_path_t = struct cairo_path;


namespace Tech {


class Gradient;
class Image;
class WindowSystemPrivate;


// WARNING! This enumeration must match to the values of the cairo_antialias_t enum
enum class AntialiasMode {
	kDefault,
	kNone,
	kGray,
	kSubPixel,
	kFast,
	kGood,
	kBest
};


enum class LinePattern {
	kSolid,
	kDash,
	kDot,
	kDashDot,
	kDashDotDot
};


// WARNING! This enumeration must match to the values of the cairo_line_cap_t enum
enum class LineCap {
	kFlat,
	kRound,
	kSquare
};


// WARNING! This enumeration must match to the values of the cairo_join_cap_t enum
enum class LineJoin {
	kMiter,
	kRound,
	kBevel
};


class Painter final {
public:
	class Path {
	public:
		~Path();
		Path(const Path& other) = delete;
		Path(Path&& other);
		Path& operator=(const Path& other) = delete;

	private:
		friend class Painter;
		cairo_path_t* path_;

		Path(cairo_path_t* path);
	};

	~Painter();

	void save();
	void restore();

	AntialiasMode antialiasMode() const;
	void setAntialiasMode(AntialiasMode mode);

	double tolerance() const;
	void setTolerance(double value);

	void beginOffscreenPaint();
	void applyOffscreenPaint();
	void discardOffscreenPaint();

	void translate(double dx, double dy);
	void scale(double sx, double sy);
	void rotate(double angle);

	Point<double> mapToDevice(const Point<double>& point) const;
	Point<double> mapDistanceToDevice(const Point<double>& point) const;
	Point<double> mapFromDevice(const Point<double>& point) const;
	Point<double> mapDistanceFromDevice(const Point<double>& point) const;

	void clip(bool preserve = false);
	void resetClip();

	Font font() const;
	void setFont(const Font& font);

	double lineWidth() const;
	void setLineWidth(double width);

	std::vector<double> linePattern() const;
	double linePatternOffset() const;
	void setLinePattern(const std::vector<double>& pattern, double offset = 0.0);
	void setLinePattern(LinePattern pattern, double offset = 0.0);

	LineCap lineCap() const;
	void setLineCap(LineCap style);

	LineJoin lineJoin() const;
	void setLineJoin(LineJoin join);

	double miterLimit() const;
	void setMiterLimit(double limit);

	bool hasCurrentPoint() const;
	Point<double> currentPoint() const;

	void moveTo(double x, double y);
	void moveTo(const Point<int>& point);
	void moveTo(const Point<double>& point);
	void moveBy(double dx, double dy);
	void moveBy(const Point<int>& delta);
	void moveBy(const Point<double>& delta);

	void lineTo(double x, double y);
	void lineTo(const Point<int>& point);
	void lineTo(const Point<double>& point);
	void lineBy(double dx, double dy);
	void lineBy(const Point<int>& delta);
	void lineBy(const Point<double>& delta);

	void curveTo(double x, double y, double cx1, double cy1, double cx2, double cy2);
	void curveTo(const Point<int>& point, const Point<int>& c1, const Point<int>& c2);

	void curveTo(const Point<double>& point, const Point<double>& c1,
			const Point<double>& c2);

	void curveBy(double dx, double dy, double cx1, double cy1, double cx2, double cy2);
	void curveBy(const Point<int>& delta, const Point<int>& c1, const Point<int>& c2);

	void curveBy(const Point<double>& delta, const Point<double>& c1,
			const Point<double>& c2);

	void arc(double x, double y, double radius, double angle1, double angle2,
			bool negative = false);

	void arc(const Point<int>& point, double radius, double angle1, double angle2,
			bool negative = false);

	void arc(const Point<double>& point, double radius, double angle1, double angle2,
			bool negative = false);

	void rectangle(double x, double y, double width, double height);
	void rectangle(const Rect<int>& rect);
	void rectangle(const Rect<double>& rect);

	void setSource(const Color& color);
	void setSource(const Gradient& gradient);
	void setSource(const Image& image);

	void stroke(bool preserve = false);
	void fill(bool preserve = false);
	void paint(double alpha = 1.0);

	Path currentPath(bool flattened = false) const;
	void path(const Path& path);

	void point(double x, double y);
	void point(const Point<int>& point);
	void point(const Point<double>& point);

	void points(const std::vector<Point<int>>& points);
	void points(const std::vector<Point<double>>& points);

	void drawImage(double x, double y, const Image& image);
	void drawImage(const Point<double>& point, const Image& image);
	void drawImage(const Rect<double>& rect, const Image& image);

	void drawText(int x, int y, const String& text);

restricted:
	Painter(cairo_surface_t* surface, PassKey<WindowSystemPrivate>);

private:
	cairo_t* context_;
	cairo_surface_t* surface_;

	struct State {
		Font font;
	};

	State state_;
	std::stack<State> states_;
};


} // namespace Tech


#endif // TECH_UI_PAINTER_H
