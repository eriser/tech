#ifndef TECH_UI_FONT_H
#define TECH_UI_FONT_H

#include <tech/passkey.h>
#include <tech/string.h>
#include <tech/ui/rect.h>


using PangoFontDescription = struct _PangoFontDescription;


namespace Tech {


class FontMetrics;
class Painter;


// WARNING! This enumeration must match to the values of the PangoWeight enum
enum class FontWeight {
	kThin       = 100,
	kUltraLight = 200,
	kLight      = 300,
	kNormal     = 400,
	kMedium     = 500,
	kSemiBold   = 600,
	kBold       = 700,
	kUltraBold  = 800,
	kHeavy      = 900,
	kUltraHeavy = 1000
};


// WARNING! This enumeration must match to the values of the PangoStretch enum
enum class FontStretch {
	kUltraCondensed,
	kExtraCondensed,
	kCondensed,
	kSemiCondensed,
	kNormal,
	kSemiExpanded,
	kExpanded,
	kExtraExpanded,
	kUltraExpanded
};


class Font final {
public:
	Font();
	Font(const String& family, int pointSize = 9);
	Font(const Font& other);
	Font(Font&& other);
	Font& operator=(const Font& other);

	~Font();

	int pointSize() const;
	void setPointSize(int size);

	int pixelSize() const;
	void setPixelSize(int size);

	bool isBold() const;
	void setBold(bool enable);

	bool isItalic() const;
	void setItalic(bool enable);

	bool isStrikeout() const;
	void setStrikeout(bool enable);

	bool isUnderline() const;
	void setUnderline(bool enable);

	FontWeight weight() const;
	void setWeight(FontWeight weight);

	FontStretch stretch() const;
	void setStretch(FontStretch stretch);

	static StringList familiesAvailable();

restricted:
	PangoFontDescription* desc(PassKey<FontMetrics, Painter>) const;

private:
	PangoFontDescription* desc_;
	bool isStrikeout_;
	bool isUnderline_;
};


} // namespace Tech


#endif // TECH_UI_FONT_H
