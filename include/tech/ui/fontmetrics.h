#ifndef TECH_UI_FONTMETRICS_H
#define TECH_UI_FONTMETRICS_H

#include <tech/string.h>
#include <tech/ui/rect.h>


using PangoLayout = struct _PangoLayout;
using PangoFontMetrics = struct _PangoFontMetrics;


namespace Tech {


class Font;


enum class ElideMode {
	kNone,
	kLeft,
	kMiddle,
	kRight
};


class FontMetrics final {
public:
	FontMetrics(const Font& font);
	~FontMetrics();

	int ascent() const;
	int descent() const;
	int height() const;

	String elidedText(const String& text, ElideMode mode, int width) const;

	Rect<int> rect(const String& text);
	int width(const String& text);
	Size<int> size(const String& text);

private:
	class Data;
	static thread_local Box<Data> data_;

	PangoLayout* layout_;
	PangoFontMetrics* metrics_;
};


} // namespace Tech


#endif // TECH_UI_FONTMETRICS_H
