#ifndef TECH_UI_LINE_H
#define TECH_UI_LINE_H

#include <cmath>
#include <type_traits>
#include "point.h"


namespace Tech {


template<typename T>
class Size;


template<typename T>
class Line {
public:
	static_assert(std::is_arithmetic<T>::value && std::is_signed<T>::value,
			"Template argument must be a signed numeric type.");

	Line();
	Line(T x1, T y1, T x2, T y2);
	Line(const Point<T>& p1, const Point<T>& p2);

	bool isNull() const;

	T x1() const;
	void setX1(T value);

	T y1() const;
	void setY1(T value);

	T x2() const;
	void setX2(T value);

	T y2() const;
	void setY2(T value);

	Point<T> begin() const;
	void setBegin(const Point<T>& point);

	Point<T> end() const;
	void setEnd(const Point<T>& point);

	void setPoints(const Point<T>& p1, const Point<T>& p2);
	void setLine(T x1, T y1, T x2, T y2);

	T dx() const;
	T dy() const;

	Size<T> deltaSize() const;

	void translate(const Point<T>& delta);
	void translate(int dx, int dy);

	Line translated(const Point<T>& delta) const;
	Line translated(int dx, int dy) const;

	T length() const;
	double angle() const;
	bool isHorizontal() const;
	bool isVertical() const;
	bool contains(const Point<T>& point, double tolerance = 0.0001) const;

	T distanceTo(Point<T>& point) const;

	// lengthFactor 0.0 equals to the begining of line and lengthFactor 1.0 equals to the
	// ending of line. Values of lengthFactor less than 0.0 or greater than 1.0 are
	// also valid.
	Point<T> pointAt(double lengthFactor) const;

	bool operator!() const;
	bool operator==(const Line& other) const;
	bool operator!=(const Line& other) const;

private:
	Point<T> begin_;
	Point<T> end_;
};


template<typename T>
Line<T>::Line()
{
}


template<typename T>
Line<T>::Line(T x1, T y1, T x2, T y2) :
	begin_(x1, y1),
	end_(x2, y2)
{
}


template<typename T>
Line<T>::Line(const Point<T>& p1, const Point<T>& p2) :
	begin_(p1),
	end_(p2)
{
}


template<typename T>
bool Line<T>::isNull() const
{
	return begin_ == end_;
}


template<typename T>
T Line<T>::x1() const
{
	return begin_.x();
}


template<typename T>
void Line<T>::setX1(T value)
{
	begin_.setX(value);
}


template<typename T>
T Line<T>::y1() const
{
	return begin_.y();
}


template<typename T>
void Line<T>::setY1(T value)
{
	begin_.setY(value);
}


template<typename T>
T Line<T>::x2() const
{
	return end_.x();
}


template<typename T>
void Line<T>::setX2(T value)
{
	end_.setX(value);
}


template<typename T>
T Line<T>::y2() const
{
	return end_.y();
}


template<typename T>
void Line<T>::setY2(T value)
{
	end_.setY(value);
}


template<typename T>
Point<T> Line<T>::begin() const
{
	return begin_;
}


template<typename T>
void Line<T>::setBegin(const Point<T>& point)
{
	begin_ = point;
}


template<typename T>
Point<T> Line<T>::end() const
{
	return end_;
}


template<typename T>
void Line<T>::setEnd(const Point<T>& point)
{
	end_ = point;
}


template<typename T>
void Line<T>::setPoints(const Point<T>& begin, const Point<T>& end)
{
	begin_ = begin;
	end_ = end;
}


template<typename T>
void Line<T>::setLine(T x1, T y1, T x2, T y2)
{
	begin_ = Point<T>(x1, y1);
	end_ = Point<T>(x2, y2);
}


template<typename T>
T Line<T>::dx() const
{
	return end_.x() - begin_.x();
}


template<typename T>
T Line<T>::dy() const
{
	return end_.y() - begin_.y();
}


template<typename T>
Size<T> Line<T>::deltaSize() const
{
	return Size<T>(dx(), dy());
}


template<typename T>
void Line<T>::translate(const Point<T>& delta)
{
	begin_ += delta;
	end_ += delta;
}


template<typename T>
void Line<T>::translate(int dx, int dy)
{
	translate(Point<T>(dx, dy));
}


template<typename T>
Line<T> Line<T>::translated(const Point<T>& delta) const
{
	return Line<T>(begin_ + delta, end_ + delta);
}


template<typename T>
Line<T> Line<T>::translated(int dx, int dy) const
{
	return translated(Point<T>(dx, dy));
}


template<typename T>
T Line<T>::length() const
{
	T dx = this->dx();
	T dy = this->dy();

	return std::sqrt(dx * dx + dy * dy);
}


template<typename T>
double Line<T>::angle() const
{
	return begin_.angleTo(end_);
}


template<typename T>
bool Line<T>::isHorizontal() const
{
	return (begin_.y() == end_.y()) && (begin_.x() != end_.x());
}


template<typename T>
bool Line<T>::isVertical() const
{
	return (begin_.x() == end_.x()) && (begin_.y() != end_.y());
}


template<typename T>
bool Line<T>::contains(const Point<T>& point, double tolerance) const
{
	double a = (end_.y() - begin_.y()) / (end_.x() - begin_.x());
	double b = begin_.y() - a * begin_.x();

	return std::abs(point.y() - (a * point.x() + b)) < tolerance;
}


template<typename T>
T Line<T>::distanceTo(Point<T>& point) const
{
	T dx = this->dx();
	T dy = this->dy();

	if(dx == 0 && dy == 0) {
		dx = point.x() - begin_.x();
		dy = point.y() - begin_.y();
	}
	else {
		T t = ((point.x() - begin_.x()) * dx + (point.y() - begin_.y()) * dy) /
				(dx * dx + dy * dy);

		if(t < 0) {
			dx = point.x() - begin_.x();
			dy = point.y() - begin_.y();
		}
		else if(t > 1) {
			dx = point.x() - end_.x();
			dy = point.y() - end_.y();
		}
		else {
			dx = point.x() - (begin_.x() + t * dx);
			dy = point.y() - (begin_.y() + t * dy);
		}
	}

	return std::sqrt(dx * dx + dy * dy);
}


template<typename T>
Point<T> Line<T>::pointAt(double lengthFactor) const
{
	return begin_ + (end_ - begin_) * lengthFactor;
}


template<typename T>
bool Line<T>::operator!() const
{
	return isNull();
}


template<typename T>
bool Line<T>::operator==(const Line& other) const
{
	return begin_ == other.begin_ && end_ == other.end_;
}


template<typename T>
bool Line<T>::operator!=(const Line& other) const
{
	return !(*this == other);
}


} // namespace Tech


#endif // TECH_UI_LINE_H
