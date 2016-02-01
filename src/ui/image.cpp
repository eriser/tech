#include <tech/ui/image.h>

#include <cairo.h>
#include <tech/logger.h>


namespace Tech {


Image::Image(int width, int height) :
	surface_(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height)),
	size_(width, height)
{
}


Image::Image(const Size<int>& size) :
	Image(size.width(), size.height())
{
}


Image::Image(const String& fileName)
{
	surface_ = cairo_image_surface_create_from_png(fileName.toUtf8());
	if(!surface_) {
		LOG("Unable to open image file '{0}'", fileName);
		return;
	}

	size_.setWidth(cairo_image_surface_get_width(surface_));
	size_.setHeight(cairo_image_surface_get_height(surface_));
}


Image::~Image()
{
	if(surface_)
		cairo_surface_finish(surface_);
}


bool Image::isNull() const
{
	return !surface_;
}


Size<int> Image::size() const
{
	return size_;
}


int Image::width() const
{
	return size_.width();
}


int Image::height() const
{
	return size_.height();
}


} // namespace Tech
