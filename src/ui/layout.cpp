#include <tech/ui/layout.h>

#include <tech/utils.h>


namespace Tech {


Layout::Layout() :
	ownerLayout_(nullptr),
	ownerWidget_(nullptr)
{
}


Layout::~Layout()
{
	if(ownerLayout_)
		ownerLayout_->remove(this);
}


Rect<int> Layout::geometry() const
{
	return geometry_;
}


void Layout::setGeometry(const Rect<int>& geometry)
{
	int width = bound(minimumSize_.width(), geometry_.width(), maximumSize_.width());
	int height = bound(minimumSize_.height(), geometry_.height(), maximumSize_.height());
	Rect<int> newGeometry = Rect<int>(geometry.x(), geometry.y(), width, height);

	if(newGeometry != geometry_) {
		geometry_ = newGeometry;
		update();
	}
}


Size<int> Layout::minimumSize() const
{
	return minimumSize_;
}


void Layout::setMinimumSize(int width, int height)
{
	minimumSize_ = Size<int>(width, height);
}


void Layout::setMinimumSize(const Size<int>& size)
{
	minimumSize_ = size;
}


Size<int> Layout::maximumSize() const
{
	return maximumSize_;
}


void Layout::setMaximumSize(int width, int height)
{
	maximumSize_ = Size<int>(width, height);
}


void Layout::setMaximumSize(const Size<int>& size)
{
	maximumSize_ = size;
}


void Layout::insert(Widget* widget, int pos)
{
	if(pos < 0 || pos > static_cast<int>(items_.size()))
		pos = items_.size();

	items_.emplace(items_.begin() + pos, Item{nullptr, widget});
}


void Layout::insert(Layout* layout, int pos)
{
	if(pos < 0 || pos > static_cast<int>(items_.size()))
		pos = items_.size();

	items_.emplace(items_.begin() + pos, Item{layout, nullptr});
}


void Layout::remove(Widget* widget)
{
	auto it = items_.begin();

	while(it != items_.end()) {
		if(it->widget == widget) {
			items_.erase(it);
			break;
		}

		++it;
	}
}


void Layout::remove(Layout* layout)
{
	auto it = items_.begin();

	while(it != items_.end()) {
		if(it->layout == layout) {
			items_.erase(it);
			delete layout;
			break;
		}

		++it;
	}
}


void Layout::removeAt(int pos)
{

}


void Layout::setOwner(Layout* owner, PassKey<Widget>)
{
	ownerLayout_ = owner;
}


void Layout::setOwner(Widget* owner, PassKey<Widget>)
{
	ownerWidget_ = owner;
}


} // namespace Tech
