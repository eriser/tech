#ifndef TECH_UI_RECT_H
#define TECH_UI_RECT_H

#include <algorithm>
#include <type_traits>
#include <tech/ui/point.h>
#include <tech/ui/size.h>


namespace Tech {


template<typename T>
class Rect {
public:
	static_assert(std::is_arithmetic<T>::value && std::is_signed<T>::value,
			"Template argument must be a signed numeric type");

	Rect();

	Rect(const Point<T>& topLeft, const Point<T>& bottomRight);
	Rect(const Point<T>& pos, const Size<T>& size);
	Rect(T x, T y, T width, T height);

	bool isNull() const;
	bool isValid() const;

	T x() const;
	void setX(T x);

	T y() const;
	void setY(T y);

	T width() const;
	void setWidth(T width);

	T height() const;
	void setHeight(T height);

	Size<T> size() const;
	void setSize(const Size<T>& size);

	T left() const;
	void setLeft(T x);
	void moveLeft(T x);

	T right() const;
	void setRight(T x);
	void moveRight(T x);

	T top() const;
	void setTop(T y);
	void moveTop(T y);

	T bottom() const;
	void setBottom(T y);
	void moveBottom(T y);

	Point<T> center() const;
	void moveCenter(T x, T y);
	void moveCenter(const Point<T>& position);

	Point<T> topLeft() const;
	void setTopLeft(T x, T y);
	void setTopLeft(const Point<T>& position);
	void moveTopLeft(T x, T y);
	void moveTopLeft(const Point<T>& position);

	Point<T> topRight() const;
	void setTopRight(T x, T y);
	void setTopRight(const Point<T>& position);
	void moveTopRight(T x, T y);
	void moveTopRight(const Point<T>& position);

	Point<T> bottomLeft() const;
	void setBottomLeft(T x, T y);
	void setBottomLeft(const Point<T>& position);
	void moveBottomLeft(T x, T y);
	void moveBottomLeft(const Point<T>& position);

	Point<T> bottomRight() const;
	void setBottomRight(T x, T y);
	void setBottomRight(const Point<T>& position);
	void moveBottomRight(T x, T y);
	void moveBottomRight(const Point<T>& position);

	void adjust(T dx1, T dy1, T dx2, T dy2);
	Rect adjusted(T dx1, T dy1, T dx2, T dy2) const;

	bool contains(T x, T y) const;
	bool contains(const Point<T>& point) const;

	bool intersects(const Rect& rect) const;

	void intersect(const Rect& other);
	Rect intersected(const Rect& other) const;

	void normalize();
	Rect normalized() const;

	void translate(int dx, int dy);
	void translate(const Point<T>& offset);
	Rect translated(int dx, int dy) const;
	Rect translated(const Point<T>& offset) const;

	void unite(const Rect& other);
	Rect united(const Rect& other) const;

	template<typename U>
	Rect<U> toRect() const;

	bool operator==(const Rect& other) const;
	bool operator!=(const Rect& other) const;

	Rect operator&(const Rect& other) const;
	Rect& operator&=(const Rect& other);

	Rect operator|(const Rect& other) const;
	Rect& operator|=(const Rect& other);

private:
	Point<T> pos_;
	Size<T> size_;
};


template<typename T>
Rect<T>::Rect()
{
}


template<typename T>
Rect<T>::Rect(const Point<T>& topLeft, const Point<T>& bottomRight) :
	pos_(topLeft),
	size_(bottomRight.x() - topLeft.x(), bottomRight.y() - topLeft.y())
{
}


template<typename T>
Rect<T>::Rect(const Point<T>& pos, const Size<T>& size) :
	pos_(pos),
	size_(size)
{
}


template<typename T>
Rect<T>::Rect(T x, T y, T width, T height) :
	pos_(x, y),
	size_(width, height)
{
}


template<typename T>
bool Rect<T>::isNull() const
{
	return size_.isNull();
}


template<typename T>
bool Rect<T>::isValid() const
{
	return size_.width() > 0 && size_.height() > 0;
}


template<typename T>
T Rect<T>::x() const
{
	return pos_.x();
}


template<typename T>
void Rect<T>::setX(T x)
{
	pos_.setX(x);
}


template<typename T>
T Rect<T>::y() const
{
	return pos_.y();
}


template<typename T>
void Rect<T>::setY(T y)
{
	pos_.setY(y);
}


template<typename T>
T Rect<T>::width() const
{
	return size_.width();
}


template<typename T>
void Rect<T>::setWidth(T width)
{
	size_.setWidth(width);
}


template<typename T>
T Rect<T>::height() const
{
	return size_.height();
}


template<typename T>
void Rect<T>::setHeight(T height)
{
	size_.setHeight(height);
}


template<typename T>
Size<T> Rect<T>::size() const
{
	return size_;
}


template<typename T>
void Rect<T>::setSize(const Size<T>& size)
{
	size_ = size;
}


template<typename T>
T Rect<T>::left() const
{
	return pos_.x();
}


template<typename T>
void Rect<T>::setLeft(T x)
{
	T diff = pos_.x() - x;
	pos_.setX(x);
	size_.setWidth(size_.width() + diff);
}


template<typename T>
void Rect<T>::moveLeft(T x)
{
	pos_.setX(x);
}


template<typename T>
T Rect<T>::right() const
{
	return pos_.x() + size_.width();
}


template<typename T>
void Rect<T>::setRight(T x)
{
	T diff = x - right();
	size_.setWidth(size_.width() + diff);
}


template<typename T>
void Rect<T>::moveRight(T x)
{
	pos_.setX(x - size_.width());
}


template<typename T>
T Rect<T>::top() const
{
	return pos_.y();
}


template<typename T>
void Rect<T>::setTop(T y)
{
	T diff = pos_.y() - y;
	pos_.setY(y);
	size_.setHeight(size_.height() + diff);
}


template<typename T>
void Rect<T>::moveTop(T y)
{
	pos_.setY(y);
}


template<typename T>
T Rect<T>::bottom() const
{
	return pos_.y() + size_.height();
}


template<typename T>
void Rect<T>::setBottom(T y)
{
	T diff = y - bottom();
	size_.setHeight(size_.height() + diff);
}


template<typename T>
void Rect<T>::moveBottom(T y)
{
	pos_.setY(y - size_.height());
}


template<typename T>
Point<T> Rect<T>::center() const
{
	return pos_ + size_.asPoint() / 2.0;
}


template<typename T>
void Rect<T>::moveCenter(T x, T y)
{
	moveCenter(Point<T>(x, y));
}


template<typename T>
void Rect<T>::moveCenter(const Point<T>& position)
{
	pos_ = position - size_.asPoint() / 2.0;
}


template<typename T>
Point<T> Rect<T>::topLeft() const
{
	return pos_;
}


template<typename T>
void Rect<T>::setTopLeft(T x, T y)
{
	pos_ = Point<T>(x, y);
}


template<typename T>
void Rect<T>::setTopLeft(const Point<T>& position)
{
	Point<T> diff = pos_ - position;
	pos_ = position;
	size_ += diff.asSize();
}


template<typename T>
void Rect<T>::moveTopLeft(T x, T y)
{
	pos_ = Point<T>(x, y);
}


template<typename T>
void Rect<T>::moveTopLeft(const Point<T>& position)
{
	pos_ = position;
}


template<typename T>
Point<T> Rect<T>::topRight() const
{
	return Point<T>(pos_.x() + size_.width(), pos_.y());
}


template<typename T>
void Rect<T>::setTopRight(T x, T y)
{
	setTopRight(Point<T>(x, y));
}


template<typename T>
void Rect<T>::setTopRight(const Point<T>& position)
{
	T diff = pos_ - position;
	pos_.setY(position.y());
	size_ += diff.asSize();
}


template<typename T>
void Rect<T>::moveTopRight(T x, T y)
{
	T diff = x - right();
	pos_ = Point<T>(pos_.x() + diff, y);
}


template<typename T>
void Rect<T>::moveTopRight(const Point<T>& position)
{
	moveTopRight(position.x(), position.y());
}


template<typename T>
Point<T> Rect<T>::bottomLeft() const
{
	return Point<T>(pos_.x(), pos_.y() + size_.height());
}


template<typename T>
void Rect<T>::setBottomLeft(T x, T y)
{
	setBottomLeft(Point<T>(x, y));
}


template<typename T>
void Rect<T>::setBottomLeft(const Point<T>& position)
{
	T diff = size_ - position.asSize();
	size_ += diff.asSize();
	pos_.setX(position.x());
}


template<typename T>
void Rect<T>::moveBottomLeft(T x, T y)
{
	T diff = y - bottom();
	pos_ = Point<T>(x, pos_.y() + diff);
}


template<typename T>
void Rect<T>::moveBottomLeft(const Point<T>& position)
{
	moveBottomLeft(position.x(), position.y());
}


template<typename T>
Point<T> Rect<T>::bottomRight() const
{
	return pos_ + size_.asPoint();
}


template<typename T>
void Rect<T>::setBottomRight(T x, T y)
{
	setBottomRight(Point<T>(x, y));
}


template<typename T>
void Rect<T>::setBottomRight(const Point<T>& position)
{
	Point<T> diff = bottomRight() - position;
	size_ += diff.asSize();
}


template<typename T>
void Rect<T>::moveBottomRight(T x, T y)
{
	moveBottomRight(Point<T>(x, y));
}


template<typename T>
void Rect<T>::moveBottomRight(const Point<T>& position)
{
	pos_ = position - size_.asPoint();
}


template<typename T>
void Rect<T>::adjust(T dx1, T dy1, T dx2, T dy2)
{
	pos_ += Point<T>(dx1, dy1);
	size_ += Size<T>(dx2 - dx1, dy2 - dy1);
}


template<typename T>
Rect<T> Rect<T>::adjusted(T dx1, T dy1, T dx2, T dy2) const
{
	return Rect<T>(pos_ + Point<T>(dx1, dy1), size_ + Size<T>(dx2 - dx1, dy2 - dy1));
}


template<typename T>
bool Rect<T>::contains(T x, T y) const
{
	return x >= left() && x < right() && y >= top() && y < bottom();
}


template<typename T>
bool Rect<T>::contains(const Point<T>& point) const
{
	return contains(point.x(), point.y());
}


template<typename T>
bool Rect<T>::intersects(const Rect<T>& rect) const
{
	return !(rect.left() > right() || rect.right() < left()
			|| rect.top() > bottom() || rect.bottom() < top());
}


template<typename T>
void Rect<T>::intersect(const Rect& other)
{
	*this = intersected(other);
}


template<typename T>
Rect<T> Rect<T>::intersected(const Rect<T>& other) const
{
	if(!intersects(other))
		return Rect<T>();

	T l = std::max(left(), other.left());
	T r = std::min(right(), other.right());
	T t = std::max(top(), other.top());
	T b = std::min(bottom(), other.bottom());

	return Rect<T>(l, t, r - l, b - t);
}


template<typename T>
void Rect<T>::normalize()
{
	if(!size_.isValid()) {
		pos_ += size_.asPoint();
		size_ = -size_;
	}
}


template<typename T>
Rect<T> Rect<T>::normalized() const
{
	if(size_.isValid())
		return *this;

	return Rect<T>(pos_ + size_.asPoint(), -size_);
}


template<typename T>
void Rect<T>::translate(int dx, int dy)
{
	*this = translated(dx, dy);
}


template<typename T>
void Rect<T>::translate(const Point<T>& offset)
{
	*this = translated(offset);
}


template<typename T>
Rect<T> Rect<T>::translated(int dx, int dy) const
{
	return translated(Point<T>(dx, dy));
}


template<typename T>
Rect<T> Rect<T>::translated(const Point<T>& offset) const
{
	return Rect<int>(pos_ + offset, size_);
}


template<typename T>
void Rect<T>::unite(const Rect<T>& other)
{
	*this = united(other);
}


template<typename T>
Rect<T> Rect<T>::united(const Rect<T>& other) const
{
	T x = std::min(pos_.x(), other.pos_.x());
	T y = std::min(pos_.y(), other.pos_.y());
	T width = std::max(right(), other.right()) - x;
	T height = std::max(bottom(), other.bottom()) - y;

	return Rect<T>(x, y, width, height);
}


template<typename T>
template<typename U>
Rect<U> Rect<T>::toRect() const
{
	static_assert(std::is_arithmetic<U>::value && std::is_signed<U>::value,
			"Template argument must be a signed numeric type");

	return Rect<U>(x(), y(), width(), height());
}


template<typename T>
bool Rect<T>::operator==(const Rect<T>& other) const
{
	return pos_ == other.pos_ && size_ == other.size_;
}


template<typename T>
bool Rect<T>::operator!=(const Rect<T>& other) const
{
	return !(*this == other);
}


template<typename T>
Rect<T> Rect<T>::operator&(const Rect<T>& other) const
{
	return intersected(other);
}


template<typename T>
Rect<T>& Rect<T>::operator&=(const Rect<T>& other)
{
	intersect(other);
	return *this;
}


template<typename T>
Rect<T> Rect<T>::operator|(const Rect<T>& other) const
{
	return united(other);
}


template<typename T>
Rect<T>& Rect<T>::operator|=(const Rect<T>& other)
{
	unite(other);
	return *this;
}


} // namespace Tech


#endif // TECH_UI_RECT_H
