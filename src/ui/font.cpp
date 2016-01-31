#include <tech/ui/font.h>

#include <pango/pangocairo.h>


namespace Tech {


Font::Font() :
	Font("Sans")
{
}


Font::Font(const String& family, int pointSize) :
	desc_(pango_font_description_from_string(family.toUtf8())),
	isStrikeout_(false),
	isUnderline_(false)
{
	setPointSize(std::max(pointSize, 1));
}


Font::Font(const Font& other) :
	desc_(pango_font_description_copy(other.desc_)),
	isStrikeout_(other.isStrikeout_),
	isUnderline_(other.isUnderline_)
{
}


Font::Font(Font&& other) :
	desc_(other.desc_),
	isStrikeout_(other.isStrikeout_),
	isUnderline_(other.isUnderline_)
{
	other.desc_ = nullptr;
	other.isStrikeout_ = false;
	other.isUnderline_ = false;
}


Font& Font::operator=(const Font& other)
{
	desc_ = pango_font_description_copy(other.desc_);
	isStrikeout_ = other.isStrikeout_;
	isUnderline_ = other.isUnderline_;
	return *this;
}


Font::~Font()
{
	pango_font_description_free(desc_);
}


int Font::pointSize() const
{
	return PANGO_PIXELS(pango_font_description_get_size(desc_));
}


void Font::setPointSize(int size)
{
	pango_font_description_set_size(desc_, size * PANGO_SCALE);
}


int Font::pixelSize() const
{
//	return PANGO_PIXELS(pango_font_description_get_asbsolute_size(desc_));
}


void Font::setPixelSize(int size)
{
	pango_font_description_set_absolute_size(desc_, size);
}


StringList Font::familiesAvailable()
{
	PangoFontMap* map = pango_cairo_font_map_get_default();

	PangoFontFamily** families;
	int count;
	pango_font_map_list_families(map, &families, &count);

	StringList result;
	for(int i = 0; i < count; ++i) {
		PangoFontFamily* family = families[i];
		result.push_back(pango_font_family_get_name(family));
	}

	free(families);
	return result;
}


PangoFontDescription* Font::desc(PassKey<FontMetrics, Painter>) const
{
	return desc_;
}


bool Font::isBold() const
{
	return pango_font_description_get_weight(desc_) == PANGO_WEIGHT_BOLD;
}


void Font::setBold(bool enable)
{
	PangoWeight weight;
	if(enable) {
		weight = PANGO_WEIGHT_BOLD;
	}
	else {
		weight = PANGO_WEIGHT_NORMAL;
	}

	pango_font_description_set_weight(desc_, weight);
}


bool Font::isItalic() const
{
	return pango_font_description_get_style(desc_) == PANGO_STYLE_ITALIC;
}


void Font::setItalic(bool enable)
{
	PangoStyle style;
	if(enable) {
		style = PANGO_STYLE_ITALIC;
	}
	else {
		style = PANGO_STYLE_NORMAL;
	}

	pango_font_description_set_style(desc_, style);
}


bool Font::isStrikeout() const
{
	return isStrikeout_;
}


void Font::setStrikeout(bool enable)
{
	isStrikeout_ = enable;
}


bool Font::isUnderline() const
{
	return isUnderline_;
}


void Font::setUnderline(bool enable)
{
	isUnderline_ = enable;
}


FontWeight Font::weight() const
{
	return static_cast<FontWeight>(pango_font_description_get_weight(desc_));
}


void Font::setWeight(FontWeight weight)
{
	PangoWeight w = static_cast<PangoWeight>(weight);
	pango_font_description_set_weight(desc_, w);
}


FontStretch Font::stretch() const
{
	return static_cast<FontStretch>(pango_font_description_get_stretch(desc_));
}


void Font::setStretch(FontStretch stretch)
{
	PangoStretch s = static_cast<PangoStretch>(stretch);
	pango_font_description_set_stretch(desc_, s);
}


} // namespace Tech
