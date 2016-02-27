#include <tech/ui/painter.h>

#include <pango/pangocairo.h>
#include <tech/ui/fontmetrics.h>
#include <tech/ui/gradient.h>
#include <tech/ui/image.h>


namespace Tech {


Painter::Path::Path(cairo_path_t* path) :
	path_(path)
{
}


Painter::Path::Path(Path&& other) :
	path_(other.path_)
{
	other.path_ = nullptr;
}


Painter::Path::~Path()
{
	if(path_)
		cairo_path_destroy(path_);
}


Painter::Painter(cairo_surface_t* surface, PassKey<WindowSystemPrivate>) :
	context_(cairo_create(surface)),
	surface_(surface)
{
	setLineWidth(1.0);
	setMiterLimit(2.0);
}


Painter::~Painter()
{
	cairo_destroy(context_);
}


void Painter::save()
{
	cairo_save(context_);
	states_.push(state_);
}


void Painter::restore()
{
	state_ = states_.top();
	states_.pop();
	cairo_restore(context_);
}


AntialiasMode Painter::antialiasMode() const
{
	return static_cast<AntialiasMode>(cairo_get_antialias(context_));
}


void Painter::setAntialiasMode(AntialiasMode mode)
{
	cairo_set_antialias(context_, static_cast<cairo_antialias_t>(mode));
}


double Painter::tolerance() const
{
	return cairo_get_tolerance(context_);
}


void Painter::setTolerance(double value)
{
	cairo_set_tolerance(context_, value);
}


void Painter::beginOffscreenPaint()
{
	cairo_push_group(context_);
	states_.push(state_);
}


void Painter::applyOffscreenPaint()
{
	state_ = states_.top();
	states_.pop();
	cairo_pop_group_to_source(context_);
	cairo_paint(context_);
}


void Painter::discardOffscreenPaint()
{
	state_ = states_.top();
	states_.pop();
	cairo_pattern_destroy(cairo_pop_group(context_));
}


void Painter::translate(double dx, double dy)
{
	cairo_translate(context_, dx, dy);
}


void Painter::scale(double sx, double sy)
{
	cairo_scale(context_, sx, sy);
}


void Painter::rotate(double angle)
{
	cairo_rotate(context_, angle);
}


Point<double> Painter::mapToDevice(const Point<double>& point) const
{
	double x = point.x();
	double y = point.y();
	cairo_user_to_device(context_, &x, &y);
	return Point<double>(x, y);
}


Point<double> Painter::mapDistanceToDevice(const Point<double>& point) const
{
	double x = point.x();
	double y = point.y();
	cairo_user_to_device_distance(context_, &x, &y);
	return Point<double>(x, y);
}


Point<double> Painter::mapFromDevice(const Point<double>& point) const
{
	double x = point.x();
	double y = point.y();
	cairo_device_to_user(context_, &x, &y);
	return Point<double>(x, y);

}


Point<double> Painter::mapDistanceFromDevice(const Point<double>& point) const
{
	double x = point.x();
	double y = point.y();
	cairo_device_to_user_distance(context_, &x, &y);
	return Point<double>(x, y);

}


void Painter::setClipRect(int x, int y, int width, int height)
{
	cairo_rectangle(context_, x, y, width, height);
	cairo_clip(context_);
}


void Painter::setClipRect(const Rect<int>& rect)
{
	setClipRect(rect.x(), rect.y(), rect.width(), rect.height());
}


void Painter::resetClip()
{
	cairo_reset_clip(context_);
}


Font Painter::font() const
{
	return state_.font;
}


void Painter::setFont(const Font& font)
{
	state_.font = font;
}


double Painter::lineWidth() const
{
	return cairo_get_line_width(context_);
}


void Painter::setLineWidth(double width)
{
	cairo_set_line_width(context_, width);
}


std::vector<double> Painter::linePattern() const
{
	int count = cairo_get_dash_count(context_);
	std::vector<double> result(count);
	cairo_get_dash(context_, &result.front(), nullptr);
	return result;
}


double Painter::linePatternOffset() const
{
	double offset;
	cairo_get_dash(context_, nullptr, &offset);
	return offset;
}


void Painter::setLinePattern(const std::vector<double>& pattern, double offset)
{
	cairo_set_dash(context_, &pattern.front(), pattern.size(), offset);
}


void Painter::setLinePattern(LinePattern pattern, double offset)
{
	double width = cairo_get_line_width(context_);

	if(pattern == LinePattern::kSolid) {
		cairo_set_dash(context_, nullptr, 0, offset);
	}
	else if(pattern == LinePattern::kDash) {
		const double kValues[2] = { width * 5.0, width * 3.0 };
		cairo_set_dash(context_, kValues, 2, offset);
	}
	else if(pattern == LinePattern::kDot) {
		const double kValues[2] = { 0.1, width * 3.0 };
		cairo_set_dash(context_, kValues, 2, offset);
	}
	else if(pattern == LinePattern::kDashDot) {
		const double kValues[4] = { width * 5.0, width * 3.0, 0.1, width * 3.0 };
		cairo_set_dash(context_, kValues, 4, offset);
	}
	else if(pattern == LinePattern::kDashDotDot) {
		const double kValues[6] = { width * 5.0, width * 3.0, 0.1, width * 3.0,
				0.1, width * 3.0 };

		cairo_set_dash(context_, kValues, 6, offset);
	}
}


LineCap Painter::lineCap() const
{
	return static_cast<LineCap>(cairo_get_line_cap(context_));
}


void Painter::setLineCap(LineCap style)
{
	cairo_set_line_cap(context_, static_cast<cairo_line_cap_t>(style));
}


LineJoin Painter::lineJoin() const
{
	return static_cast<LineJoin>(cairo_get_line_join(context_));
}


void Painter::setLineJoin(LineJoin join)
{
	cairo_set_line_join(context_, static_cast<cairo_line_join_t>(join));
}


double Painter::miterLimit() const
{
	return cairo_get_miter_limit(context_);
}


void Painter::setMiterLimit(double limit)
{
	cairo_set_miter_limit(context_, limit);
}


void Painter::point(double x, double y)
{
	cairo_line_cap_t cap = cairo_get_line_cap(context_);
	cairo_set_line_cap(context_, CAIRO_LINE_CAP_ROUND);

	cairo_move_to(context_, x, y);
	cairo_line_to(context_, x, y);
	cairo_stroke(context_);

	cairo_set_line_cap(context_, cap);
}


void Painter::point(const Point<int>& point)
{
	this->point(point.x(), point.y());
}


void Painter::point(const Point<double>& point)
{
	this->point(point.x(), point.y());
}


void Painter::points(const std::vector<Point<int>>& points)
{
	cairo_line_cap_t cap = cairo_get_line_cap(context_);
	cairo_set_line_cap(context_, CAIRO_LINE_CAP_ROUND);

	for(const Point<int>& point : points) {
		cairo_move_to(context_, point.x(), point.y());
		cairo_line_to(context_, point.x(), point.y());
	}

	cairo_set_line_cap(context_, cap);
}


void Painter::points(const std::vector<Point<double>>& points)
{
	cairo_line_cap_t cap = cairo_get_line_cap(context_);
	cairo_set_line_cap(context_, CAIRO_LINE_CAP_ROUND);

	for(const Point<double>& point : points) {
		cairo_move_to(context_, point.x(), point.y());
		cairo_line_to(context_, point.x(), point.y());
	}

	cairo_set_line_cap(context_, cap);
}


bool Painter::hasCurrentPoint() const
{
	return cairo_has_current_point(context_) != 0;
}


Point<double> Painter::currentPoint() const
{
	double x, y;
	cairo_get_current_point(context_, &x, &y);
	return Point<double>(x, y);
}


void Painter::moveTo(double x, double y)
{
	cairo_move_to(context_, x, y);
}


void Painter::moveTo(const Point<int>& point)
{
	cairo_move_to(context_, point.x(), point.y());
}


void Painter::moveTo(const Point<double>& point)
{
	cairo_move_to(context_, point.x(), point.y());
}


void Painter::moveBy(double dx, double dy)
{
	cairo_rel_move_to(context_, dx, dy);
}


void Painter::moveBy(const Point<int>& delta)
{
	cairo_rel_move_to(context_, delta.x(), delta.y());
}


void Painter::moveBy(const Point<double>& delta)
{
	cairo_rel_move_to(context_, delta.x(), delta.y());
}


void Painter::lineTo(double x, double y)
{
	cairo_line_to(context_, x, y);
}


void Painter::lineTo(const Point<int>& point)
{
	cairo_line_to(context_, point.x(), point.y());
}


void Painter::lineTo(const Point<double>& point)
{
	cairo_line_to(context_, point.x(), point.y());
}


void Painter::lineBy(double dx, double dy)
{
	cairo_rel_line_to(context_, dx, dy);
}


void Painter::lineBy(const Point<int>& delta)
{
	cairo_rel_line_to(context_, delta.x(), delta.y());
}


void Painter::lineBy(const Point<double>& delta)
{
	cairo_rel_line_to(context_, delta.x(), delta.y());
}


void Painter::curveTo(double x, double y, double cx1, double cy1, double cx2, double cy2)
{
	cairo_curve_to(context_, x, y, cx1, cy1, cx2, cy2);
}


void Painter::curveTo(const Point<int>& point, const Point<int>& c1, const Point<int>& c2)
{
	cairo_curve_to(context_, point.x(), point.y(), c1.x(), c1.y(), c2.x(), c2.y());
}


void Painter::curveTo(const Point<double>& point, const Point<double>& c1,
		const Point<double>& c2)
{
	cairo_curve_to(context_, point.x(), point.y(), c1.x(), c1.y(), c2.x(), c2.y());
}


void Painter::curveBy(double dx, double dy, double cx1, double cy1, double cx2,
		double cy2)
{
	cairo_rel_curve_to(context_, dx, dy, cx1, cy1, cx2, cy2);
}


void Painter::curveBy(const Point<int>& delta, const Point<int>& c1, const Point<int>& c2)
{
	cairo_rel_curve_to(context_, delta.x(), delta.y(), c1.x(), c1.y(), c2.x(), c2.y());
}


void Painter::curveBy(const Point<double>& delta, const Point<double>& c1,
		const Point<double>& c2)
{
	cairo_rel_curve_to(context_, delta.x(), delta.y(), c1.x(), c1.y(), c2.x(), c2.y());
}


void Painter::arc(double x, double y, double radius, double angle1, double angle2,
		bool negative)
{
	if(negative) {
		cairo_arc_negative(context_, x, y, radius, angle1, angle2);
	}
	else {
		cairo_arc(context_, x, y, radius, angle1, angle2);
	}
}


void Painter::arc(const Point<int>& point, double radius, double angle1, double angle2,
		bool negative)
{
	if(negative) {
		cairo_arc_negative(context_, point.x(), point.y(), radius, angle1, angle2);
	}
	else {
		cairo_arc(context_, point.x(), point.y(), radius, angle1, angle2);
	}
}


void Painter::arc(const Point<double>& point, double radius, double angle1, double angle2,
		bool negative)
{
	if(negative) {
		cairo_arc_negative(context_, point.x(), point.y(), radius, angle1, angle2);
	}
	else {
		cairo_arc(context_, point.x(), point.y(), radius, angle1, angle2);
	}
}


void Painter::rectangle(double x, double y, double width, double height)
{
	cairo_rectangle(context_, x, y, width, height);
}


void Painter::rectangle(const Rect<int>& rect)
{
	cairo_rectangle(context_, rect.x(), rect.y(), rect.width(), rect.height());
}


void Painter::rectangle(const Rect<double>& rect)
{
	cairo_rectangle(context_, rect.x(), rect.y(), rect.width(), rect.height());
}


void Painter::setSource(const Color& color)
{
	cairo_set_source_rgba(context_, color.red_, color.green_, color.blue_, color.alpha_);
}


void Painter::setSource(const Gradient& gradient)
{
	cairo_set_source(context_, gradient.pattern_);
}


void Painter::setSource(const Image& image)
{
	cairo_set_source_surface(context_, image.surface_, 0, 0);
}


void Painter::stroke(bool preserve)
{
	if(preserve) {
		cairo_stroke_preserve(context_);
	}
	else {
		cairo_stroke(context_);
	}
}


void Painter::fill(bool preserve)
{
	if(preserve) {
		cairo_fill_preserve(context_);
	}
	else {
		cairo_fill(context_);
	}
}


void Painter::paint(double alpha)
{
	cairo_paint_with_alpha(context_, alpha);
}


Painter::Path Painter::currentPath(bool flattened) const
{
	if(flattened)
		return Path(cairo_copy_path_flat(context_));

	return Path(cairo_copy_path(context_));
}


void Painter::path(const Path& path)
{
	cairo_append_path(context_, path.path_);
}


void Painter::drawImage(double x, double y, const Image& image)
{
	cairo_set_source_surface(context_, image.surface_, x, y);
	cairo_paint(context_);
}


void Painter::drawImage(const Point<double>& point, const Image& image)
{
	cairo_set_source_surface(context_, image.surface_, point.x(), point.y());
	cairo_paint(context_);
}


void Painter::drawImage(const Rect<double>& rect, const Image& image)
{

}


void Painter::drawText(int x, int y, const String& text)
{
	PangoLayout* layout = pango_cairo_create_layout(context_);
	pango_layout_set_text(layout, text.toUtf8(), text.length());

	pango_layout_set_font_description(layout, state_.font.desc({}));

	if(state_.font.isStrikeout() || state_.font.isUnderline()) {
		PangoAttrList* attrs = pango_attr_list_new();

		if(state_.font.isStrikeout()) {
			PangoAttribute* attr = pango_attr_strikethrough_new(true);
			pango_attr_list_insert(attrs, attr);
		}

		if(state_.font.isUnderline()) {
			PangoAttribute* attr = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
			pango_attr_list_insert(attrs, attr);
		}

		pango_layout_set_attributes(layout, attrs);
		pango_attr_list_unref(attrs);
	}

	cairo_move_to(context_, x, y);
	pango_cairo_update_layout(context_, layout);
//	pango_cairo_layout_path(context_, layout);
	pango_cairo_show_layout(context_, layout);

	g_object_unref(layout);
}


} // namespace Tech
