#ifndef TECH_CIRCBUFFER_H
#define TECH_CIRCBUFFER_H

#include <algorithm>
#include <type_traits>


namespace Tech {


template<class T>
class CircBuffer {
public:
	CircBuffer(size_t capacity);
	CircBuffer(const CircBuffer& other);
	CircBuffer(CircBuffer&& other);

	~CircBuffer();

	CircBuffer& operator=(const CircBuffer& other);

	bool isEmpty() const;
	bool isFull() const;

	size_t count() const;
	size_t capacity() const;
	size_t space() const;

	void clear();
	void resize(size_t newCapacity);
	void swap(CircBuffer<T>& other);

	T& front();
	constexpr const T& front() const;

	T& back();
	const T& back() const;

	T& operator[](size_t pos);
	const T& operator[](size_t pos) const;

	void push(const T& value);
	size_t push(const T* buffer, size_t maxCount);

	T pop();
	size_t pop(T* buffer, size_t maxCount);

	bool peekChunk(const T** buffer, size_t* count);

	void remove(size_t count);

private:
	size_t capacity_;
	size_t count_;
	size_t first_;
	T* data_;

	size_t addAndWrap(size_t pos, size_t count) const;
};


template<class T>
CircBuffer<T>::CircBuffer(size_t capacity)
{
	static_assert(std::is_trivial<T>::value,
			"The template parameter should be a trivial type.");

	capacity_ = std::max(capacity, static_cast<size_t>(1));
	data_     = new T[capacity_];
	count_    = 0;
	first_    = 0;
}


template<class T>
CircBuffer<T>::CircBuffer(const CircBuffer<T>& other)
{
	capacity_ = other.capacity_;
	count_    = other.count_;
	first_    = other.first_;
	data_     = new T[capacity_];

	std::copy(other.data_, other.data_ + other.capacity_, data_);
}


template<class T>
CircBuffer<T>::CircBuffer(CircBuffer<T>&& other)
{
	capacity_ = other.capacity_;
	count_    = other.count_;
	data_     = other.data_;
	first_    = other.first_;

	other.data_     = nullptr;
	other.capacity_ = 0;
}


template<class T>
CircBuffer<T>::~CircBuffer()
{
	if(data_)
		delete [] data_;
}


template<class T>
CircBuffer<T>& CircBuffer<T>::operator=(const CircBuffer<T>& other)
{
	if(this == &other)
		return *this;

	delete [] data_;

	capacity_ = other.capacity_;
	count_    = other.count_;
	first_    = other.first_;
	data_     = new T[capacity_];
	std::copy(other.data_, other.data_ + other.capacity_, data_);

	return *this;
}


template<class T>
bool CircBuffer<T>::isEmpty() const
{
	return count_ == 0;
}


template<class T>
bool CircBuffer<T>::isFull() const
{
	return count_ == capacity_;
}


template<class T>
size_t CircBuffer<T>::count() const
{
	return count_;
}


template<class T>
size_t CircBuffer<T>::capacity() const
{
	return capacity_;
}


template<class T>
size_t CircBuffer<T>::space() const
{
	return capacity_ - count_;
}


template<class T>
void CircBuffer<T>::clear()
{
	count_ = 0;
}


template<class T>
void CircBuffer<T>::resize(size_t newCapacity)
{
	newCapacity = std::max(newCapacity, static_cast<size_t>(1));
	T* newData  = new T[newCapacity];

	if(newCapacity < count_) {
		first_ = addAndWrap(first_, count_ - newCapacity);
		count_ -= newCapacity;
	}

	T* output = newData;
	size_t count = std::min(capacity_ - first_, count_);
	if(count < count_) {
		output = std::copy(data_ + first_, data_ + first_ + count, output);
		first_ = addAndWrap(first_, count);
		count = count_ - count;
	}

	std::copy(data_ + first_, data_ + first_ + count, output);

	delete [] data_;

	data_     = newData;
	capacity_ = newCapacity;
	first_    = 0;
}


template<class T>
void CircBuffer<T>::swap(CircBuffer<T>& other)
{
	std::swap(data_, other.data_);
	std::swap(capacity_, other.capacity_);
	std::swap(count_, other.count_);
	std::swap(first_, other.first_);
}


template<class T>
T& CircBuffer<T>::front()
{
	return data_[first_];
}


template<class T>
constexpr const T& CircBuffer<T>::front() const
{
	return data_[first_];

}


template<class T>
T& CircBuffer<T>::back()
{
	size_t pos = addAndWrap(first_, count_ - 1);
	return data_[pos];
}


template<class T>
const T& CircBuffer<T>::back() const
{
	size_t pos = addAndWrap(first_, count_ - 1);
	return data_[pos];
}


template<class T>
T& CircBuffer<T>::operator[](size_t pos)
{
	pos = addAndWrap(first_, pos);
	return data_[pos];
}


template<class T>
const T& CircBuffer<T>::operator[](size_t pos) const
{
	pos = addAndWrap(first_, pos);
	return data_[pos];
}


template<class T>
void CircBuffer<T>::push(const T& value)
{
	size_t pos = addAndWrap(first_, count_);
	data_[pos] = value;

	if(count_ < capacity_)
		count_++;
	else
		first_ = addAndWrap(first_, 1);
}


template<class T>
size_t CircBuffer<T>::push(const T* buffer, size_t maxCount)
{
	if(maxCount > capacity_) {
		buffer += maxCount - capacity_;
		maxCount = capacity_;
	}

	size_t pos = addAndWrap(first_, count_);
	size_t count = std::min(capacity_ - pos, maxCount);

	if(count < maxCount) {
		std::copy(buffer, buffer + count, data_ + pos);
		buffer += count;
		count = maxCount - count;
		pos = 0;
	}

	std::copy(buffer, buffer + count, data_ + pos);
	count_ = std::min(count_ + maxCount, capacity_);

	return maxCount;
}


template<class T>
T CircBuffer<T>::pop()
{
	if(isEmpty())
		return T();

	size_t pos = first_;
	first_ = addAndWrap(first_, 1);
	count_--;

	return data_[pos];
}


template<class T>
size_t CircBuffer<T>::pop(T* buffer, size_t maxCount)
{
	maxCount = std::min(maxCount, count_);
	if(!maxCount)
		return 0;

	size_t end = addAndWrap(first_, count_);

	if(end < first_) {
		buffer = std::copy(data_ + first_, data_ + capacity_, buffer);
		first_ = 0;
	}

	std::copy(data_ + first_, data_ + end, buffer);
	first_ = end;
	count_ -= maxCount;
	return maxCount;
}


template<class T>
bool CircBuffer<T>::peekChunk(const T** buffer, size_t* count)
{
	if(isEmpty())
		return false;

	*buffer = data_ + first_;

	size_t end = addAndWrap(first_, count_);
	if(end < first_) {
		*count = capacity_ - first_;
	}
	else {
		*count = end;
	}

	return true;
}


template<class T>
void CircBuffer<T>::remove(size_t count)
{
	count = std::min(count, count_);
	if(count) {
		first_ = addAndWrap(first_, count);
		count_ -= count;
	}
}


template<class T>
size_t CircBuffer<T>::addAndWrap(size_t pos, size_t count) const
{
	pos += count;
	if(pos < capacity_)
		return pos;

	return pos % capacity_;
}


} // namespace Tech


#endif // TECH_CIRCBUFFER_H
