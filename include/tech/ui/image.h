#ifndef TECH_UI_IMAGE_H
#define TECH_UI_IMAGE_H

#include <tech/string.h>
#include <tech/ui/size.h>

using cairo_surface_t = struct _cairo_surface;


namespace Tech {


class Image final {
public:
	Image(int width, int height);
	Image(const Size<int>& size);
	Image(const String& fileName);
	~Image();

	bool isNull() const;

	Size<int> size() const;
	int width() const;
	int height() const;

private:
	friend class Painter;

	cairo_surface_t* surface_;
	Size<int> size_;
};


} // namespace Tech


#endif // TECH_UI_IMAGE_H
