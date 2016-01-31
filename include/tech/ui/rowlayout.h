#ifndef TECH_UI_ROWLAYOUT_H
#define TECH_UI_ROWLAYOUT_H

#include <tech/ui/layout.h>


namespace Tech {


class RowLayout : public Layout {
public:
	enum Kind {
		kHorizontal,
		kVertical
	};

	RowLayout(Kind kind);

	void update() override;

private:
	Kind kind_;
};


} // namespace Tech


#endif // TECH_UI_ROWLAYOUT_H
