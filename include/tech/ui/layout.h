#ifndef TECH_UI_LAYOUT_H
#define TECH_UI_LAYOUT_H

#include <vector>
#include <tech/passkey.h>
#include <tech/ui/rect.h>


namespace Tech {


class Widget;


class Layout {
public:
	Layout();
	virtual ~Layout();

	void insert(Widget* widget, int pos = -1);
	void insert(Layout* layout, int pos = -1);
	void remove(Widget* widget);
	void remove(Layout* layout);
	void removeAt(int pos);

	virtual void update() = 0;

	Rect<int> geometry() const;
	void setGeometry(const Rect<int>& geometry);

	Size<int> minimumSize() const;
	void setMinimumSize(int width, int height);
	void setMinimumSize(const Size<int>& size);

	Size<int> maximumSize() const;
	void setMaximumSize(int width, int height);
	void setMaximumSize(const Size<int>& size);

restricted:
	void setOwner(Layout* owner, PassKey<Widget>);
	void setOwner(Widget* owner, PassKey<Widget>);

protected:

private:
	struct Item {
		Layout* layout;
		Widget* widget;
	};

	Layout* ownerLayout_;
	Widget* ownerWidget_;
	std::vector<Item> items_;

	Rect<int> geometry_;
	Size<int> minimumSize_;
	Size<int> maximumSize_;
};


} // namespace Tech


#endif // TECH_UI_LAYOUT_H
