#ifndef TECH_UI_SIZE_H
#define TECH_UI_SIZE_H

#include <algorithm>
#include <type_traits>


namespace Tech {


template<typename T>
class Point;


template<typename T>
class Size {
public:
	static_assert(std::is_arithmetic<T>::value && std::is_signed<T>::value,
			"Template argument must be a signed numeric type");

	Size();
	Size(T width, T height);

	bool isNull() const;
	bool isEmpty() const;
	bool isValid() const;

	T width() const;
	void setWidth(T value);

	T height() const;
	void setHeight(T value);

	Size boundedTo(const Size& other) const;
	Size expandedTo(const Size& other) const;

	void transpose();
	Size transposed() const;

	template<typename U>
	Size<U> toSize() const;

	Point<T> asPoint() const;

	bool operator!() const;
	Size operator-() const;

	bool operator==(const Size& other) const;
	bool operator!=(const Size& other) const;

	Size& operator+=(const Size& other);
	Size& operator-=(const Size& other);
	Size& operator*=(double factor);
	Size& operator*=(const Size& other);
	Size& operator/=(double divisor);
	Size& operator/=(const Size& other);

	const Size operator+(const Size& other) const;
	const Size operator-(const Size& other) const;
	const Size operator*(const Size& other) const;
	const Size operator/(const Size& other) const;
	const Size operator*(double factor) const;
	const Size operator/(double divisor) const;

private:
	T width_;
	T height_;
};


template<typename T>
Size<T>::Size() :
	width_(T()),
	height_(T())
{
}


template<typename T>
Size<T>::Size(T width, T height) :
	width_(width),
	height_(height)
{
}


template<typename T>
bool Size<T>::isNull() const
{
	return (width_ == T()) && (height_ == T());
}


template<typename T>
bool Size<T>::isEmpty() const
{
	return (width_ <= T()) || (height_ <= T());
}


template<typename T>
bool Size<T>::isValid() const
{
	return (width_ >= T()) && (height_ >= T());
}


template<typename T>
T Size<T>::width() const
{
	return width_;
}


template<typename T>
void Size<T>::setWidth(T value)
{
	width_ = value;
}


template<typename T>
T Size<T>::height() const
{
	return height_;
}


template<typename T>
void Size<T>::setHeight(T value)
{
	height_ = value;
}


template<typename T>
Size<T> Size<T>::boundedTo(const Size<T>& other) const
{
	return Size<T>(std::min(width(), other.width()), std::min(height(), other.height()));
}


template<typename T>
Size<T> Size<T>::expandedTo(const Size<T>& other) const
{
	return Size<T>(std::max(width(), other.width()), std::max(height(), other.height()));
}


template<typename T>
void Size<T>::transpose()
{
	std::swap(width_, height_);
}


template<typename T>
Size<T> Size<T>::transposed() const
{
	return Size<T>(height_, width_);
}


template<typename T>
template<typename U>
Size<U> Size<T>::toSize() const
{
	static_assert(std::is_arithmetic<U>::value && std::is_signed<U>::value,
			"Template argument must be a signed numeric type");

	return Size<U>(width_, height_);
}


template<typename T>
Point<T> Size<T>::asPoint() const
{
	return Point<T>(width_, height_);
}


template<typename T>
bool Size<T>::operator!() const
{
	return isNull();
}


template<typename T>
Size<T> Size<T>::operator-() const
{
	return Size<T>(-width_, -height_);
}


template<typename T>
bool Size<T>::operator==(const Size<T>& other) const
{
	return width_ == other.width_ && height_ == other.height_;
}


template<typename T>
bool Size<T>::operator!=(const Size<T>& other) const
{
	return !(*this == other);
}


template<typename T>
Size<T>& Size<T>::operator+=(const Size<T>& other)
{
	width_ += other.width_;
	height_ += other.height_;
	return *this;
}


template<typename T>
Size<T>& Size<T>::operator-=(const Size<T>& other)
{
	width_ -= other.width_;
	height_ -= other.height_;
	return *this;
}


template<typename T>
Size<T>& Size<T>::operator*=(double factor)
{
	width_ *= factor;
	height_ *= factor;
	return *this;
}


template<typename T>
Size<T>& Size<T>::operator*=(const Size<T>& other)
{
	width_ *= other.width_;
	height_ *= other.height_;
	return *this;
}


template<typename T>
Size<T>& Size<T>::operator/=(double divisor)
{
	width_ /= divisor;
	height_ /= divisor;
	return *this;
}


template<typename T>
Size<T>& Size<T>::operator/=(const Size<T>& other)
{
	width_ /= other.width_;
	height_ /= other.height_;
	return *this;
}


template<typename T>
const Size<T> Size<T>::operator+(const Size<T>& other) const
{
	Size<T> result = *this;
	result += other;
	return result;
}


template<typename T>
const Size<T> Size<T>::operator-(const Size<T>& other) const
{
	Size<T> result = *this;
	result += other;
	return result;
}


template<typename T>
const Size<T> Size<T>::operator*(const Size<T>& other) const
{
	Size<T> result = *this;
	result *= other;
	return result;
}


template<typename T>
const Size<T> Size<T>::operator/(const Size<T>& other) const
{
	Size<T> result = *this;
	result /= other;
	return result;
}


template<typename T>
const Size<T> operator*(double factor, const Size<T>& size)
{
	Size<T> result = size;
	result *= factor;
	return result;
}


template<typename T>
const Size<T> Size<T>::operator*(double factor) const
{
	return Size<T>(width_ * factor, height_ * factor);
}


template<typename T>
const Size<T> Size<T>::operator/(double divisor) const
{
	return Size<T>(width_ / divisor, height_ / divisor);
}


} // namespace Tech


#endif // TECH_UI_SIZE_H
