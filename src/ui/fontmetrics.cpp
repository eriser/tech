#include <tech/ui/fontmetrics.h>

#include <tech/logger.h>
#include <tech/ui/font.h>
#include <pango/pangocairo.h>


namespace Tech {


class FontMetrics::Data {
public:
	Data()
	{
		fontMap = pango_cairo_font_map_get_default();
		context = pango_font_map_create_context(fontMap);
	}

	~Data()
	{
		g_object_unref(context);
	}

	PangoFontMap* fontMap;
	PangoContext* context;
};


/*thread_local*/ Box<FontMetrics::Data> FontMetrics::data_ = makeBox<FontMetrics::Data>();


FontMetrics::FontMetrics(const Font& font)
{
	layout_ = pango_layout_new(data_->context);
	pango_layout_set_font_description(layout_, font.desc({}));

	PangoLanguage* language = pango_language_get_default();
	metrics_ = pango_context_get_metrics(data_->context, font.desc({}), language);
}


FontMetrics::~FontMetrics()
{
	if(layout_)
		g_object_unref(layout_);

	if(metrics_)
		pango_font_metrics_unref(metrics_);
}


Rect<int> FontMetrics::rect(const String& text)
{
	pango_layout_set_text(layout_, text.toUtf8(), text.length());

	PangoRectangle rect;
	pango_layout_get_pixel_extents(layout_, nullptr, &rect);
	return Rect<int>(rect.x, rect.y, rect.width, rect.height);
}


int FontMetrics::width(const String& text)
{
	pango_layout_set_text(layout_, text.toUtf8(), text.length());

	PangoRectangle rect;
	pango_layout_get_pixel_extents(layout_, nullptr, &rect);
	return rect.width;
}


Size<int> FontMetrics::size(const String& text)
{
	pango_layout_set_text(layout_, text.toUtf8(), text.length());

	int width;
	int height;
	pango_layout_get_pixel_size(layout_, &width, &height);
	return Size<int>(width, height);
}


int FontMetrics::ascent() const
{
	return PANGO_PIXELS(pango_font_metrics_get_ascent(metrics_));
}


int FontMetrics::descent() const
{
	return PANGO_PIXELS(pango_font_metrics_get_descent(metrics_));
}


int FontMetrics::height() const
{
	return PANGO_PIXELS(pango_font_metrics_get_ascent(metrics_)
		   + pango_font_metrics_get_descent(metrics_));
}


String FontMetrics::elidedText(const String& text, ElideMode mode, int width) const
{
	pango_layout_set_ellipsize(layout_, PANGO_ELLIPSIZE_END);
	pango_layout_set_width(layout_, width * PANGO_SCALE);
	pango_layout_set_text(layout_, text.toUtf8(), text.length());
	PangoLayoutLine* line = pango_layout_get_line_readonly(layout_, 0);
/*
	GSList* item = line->runs;
	while(item) {
		PangoLayoutRun* run = static_cast<PangoLayoutRun*>(item->data);
		LOG("run->item->offset=%d", run->item->offset);
		LOG("run->item->length=%d", run->item->length);
		LOG("run->item->num_chars=%d", run->item->num_chars);

		LOG("run->glyphs->num_glyphs=%d", run->glyphs->num_glyphs);

		LOG("----------------------------------------");

		item = item->next;
	}
*/

	PangoLayoutRun* run = static_cast<PangoLayoutRun*>(line->runs->data);
	uint length = static_cast<uint>(run->glyphs->num_glyphs);

	if(run->item->analysis.flags && PANGO_ANALYSIS_FLAG_IS_ELLIPSIS)
		return u"…";

	pango_layout_set_ellipsize(layout_, PANGO_ELLIPSIZE_NONE);
	pango_layout_set_width(layout_, -1);

	if(length == text.length())
		return text;

	if(mode == ElideMode::kLeft) {
		return u'…' + text.left(length);
	}
	else if(mode == ElideMode::kRight) {
		return text.left(length) + u'…';
	}
	else if(mode == ElideMode::kMiddle) {
		uint half = length / 2;
		return text.left(half) + u'…' + text.middle(half, length - half);
	}

	return text;
}


} // namespace Tech
