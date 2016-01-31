#ifndef TECH_UI_POINT_H
#define TECH_UI_POINT_H

#include <cmath>
#include <type_traits>


namespace Tech {


template<typename T>
class Size;


template<typename T>
class Point {
public:
	static_assert(std::is_arithmetic<T>::value && std::is_signed<T>::value,
			"Template argument must be a signed numeric type");

	/**
	 * Creates null point.
	 */
	Point();
	Point(T x, T y);

	bool isNull() const;

	T x() const;
	T y() const;

	void setX(T value);
	void setY(T value);

	T manhattanLength() const;
	double distanceFrom(const Point& other) const;

	double angleTo(const Point& other) const;

	Point rotatedAbout(const Point& origin, double angle);

	Point circumfencePoint(double radius, double angle);
	Point circumfencePoint(double radiusX, double radiusY, double angle);

	template<typename U>
	Point<U> toPoint() const;

	Size<T> asSize() const;

	bool operator!() const;
	Point operator-() const;

	bool operator==(const Point& other) const;
	bool operator!=(const Point& other) const;

	Point& operator+=(const Point& other);
	Point& operator-=(const Point& other);
	Point& operator*=(double factor);
	Point& operator*=(const Point& other);
	Point& operator/=(double divisor);
	Point& operator/=(const Point& other);

	const Point operator+(const Point& other) const;
	const Point operator-(const Point& other) const;
	const Point operator*(const Point& other) const;
	const Point operator/(const Point& other) const;
	const Point operator*(double factor) const;
	const Point operator/(double divisor) const;

private:
	T x_;
	T y_;
};


template<typename T>
Point<T>::Point() :
	x_(T()),
	y_(T())
{
}


template<typename T>
Point<T>::Point(T x, T y) :
	x_(x),
	y_(y)
{
}


template<typename T>
bool Point<T>::isNull() const
{
	return (x_ == T()) && (y_ == T());
}


template<typename T>
T Point<T>::x() const
{
	return x_;
}


template<typename T>
T Point<T>::y() const
{
	return y_;
}


template<typename T>
void Point<T>::setX(T value)
{
	x_ = value;
}


template<typename T>
void Point<T>::setY(T value)
{
	y_ = value;
}


template<typename T>
T Point<T>::manhattanLength() const
{
	return x_ + y_;
}


template<typename T>
double Point<T>::distanceFrom(const Point<T>& other) const
{
	T dx = x_ - other.x_;
	T dy = y_ - other.y_;

	return std::sqrt(dx * dx + dy * dy);
}


template<typename T>
double Point<T>::angleTo(const Point<T>& other) const
{
	return std::atan2(other.x_ - x_, y_ - other.y_);
}


template<typename T>
Point<T> Point<T>::rotatedAbout(const Point<T>& origin, double angle)
{
	T x = static_cast<double>(x_) * std::cos(angle) - y_ * std::sin(angle);
	T y = static_cast<double>(x_) * std::sin(angle) + y_ * std::sin(angle);

	return Point<T>(x, y) + origin;
}


template<typename T>
Point<T> Point<T>::circumfencePoint(double radius, double angle)
{
	return Point(x_ + radius * std::sin(angle), y_ - radius * std::cos(angle));
}


template<typename T>
Point<T> Point<T>::circumfencePoint(double radiusX, double radiusY,	double angle)
{
	return Point(x_ + radiusX * std::sin(angle), y_ - radiusY * std::cos(angle));
}


template<typename T>
template<typename U>
Point<U> Point<T>::toPoint() const
{
	static_assert(std::is_arithmetic<U>::value && std::is_signed<U>::value,
			"Template argument must be a signed numeric type");

	return Point<U>(x_, y_);
}


template<typename T>
Size<T> Point<T>::asSize() const
{
	return Size<T>(x_, y_);
}


template<typename T>
bool Point<T>::operator!() const
{
	return isNull();
}


template<typename T>
Point<T> Point<T>::operator-() const
{
	return Point<T>(-x_, -y_);
}


template<typename T>
bool Point<T>::operator==(const Point<T>& other) const
{
	return x_ == other.x_ && y_ == other.y_;
}


template<typename T>
bool Point<T>::operator!=(const Point<T>& other) const
{
	return !(*this == other);
}


template<typename T>
Point<T>& Point<T>::operator+=(const Point<T>& other)
{
	x_ += other.x_;
	y_ += other.y_;
	return *this;
}


template<typename T>
Point<T>& Point<T>::operator-=(const Point<T>& other)
{
	x_ -= other.x_;
	y_ -= other.y_;
	return *this;
}


template<typename T>
Point<T>& Point<T>::operator*=(double factor)
{
	x_ *= factor;
	y_ *= factor;
	return *this;
}


template<typename T>
Point<T>& Point<T>::operator*=(const Point<T>& other)
{
	x_ *= other.x_;
	y_ *= other.y_;
	return *this;
}


template<typename T>
Point<T>& Point<T>::operator/=(double divisor)
{
	x_ /= divisor;
	y_ /= divisor;
	return *this;
}


template<typename T>
Point<T>& Point<T>::operator/=(const Point<T>& other)
{
	x_ /= other.x_;
	y_ /= other.y_;
	return *this;
}


template<typename T>
const Point<T> Point<T>::operator+(const Point<T>& other) const
{
	Point<T> result = *this;
	result += other;
	return result;
}


template<typename T>
const Point<T> Point<T>::operator-(const Point<T>& other) const
{
	Point<T> result = *this;
	result -= other;
	return result;
}


template<typename T>
const Point<T> Point<T>::operator*(const Point<T>& other) const
{
	Point<T> result = *this;
	result *= other;
	return result;
}


template<typename T>
const Point<T> Point<T>::operator/(const Point<T>& other) const
{
	Point<T> result = *this;
	result /= other;
	return result;
}


template<typename T>
const Point<T> operator*(double factor, const Point<T>& point)
{
	Point<T> result = point;
	result *= factor;
	return result;
}


template<typename T>
const Point<T> Point<T>::operator*(double factor) const
{
	return Point<T>(x_ * factor, y_ * factor);
}


template<typename T>
const Point<T> Point<T>::operator/(double divisor) const
{
	return Point<T>(x_ / divisor, y_ / divisor);
}


} // namespace Tech


#endif // TECH_UI_POINT_H
