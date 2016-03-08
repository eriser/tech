#ifndef TECH_UI_STYLE_H
#define TECH_UI_STYLE_H


namespace Tech {


class Painter;
class Widget;


class StyleOptions {
public:

};


enum class StyleElement {
	kButton
};


class Style {
public:
	virtual ~Style() = default;

	virtual void drawElement(const StyleOptions* options, Painter* painter,
			const Widget* widget = nullptr);

protected:

private:

};



} // namespace Tech


#endif // TECH_UI_STYLE_H
