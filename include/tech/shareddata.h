#ifndef TECH_SHAREDDATA_H
#define TECH_SHAREDDATA_H

#include <algorithm>
#include <atomic>
#include <type_traits>


namespace Tech {


class SharedData {
public:
	mutable std::atomic_uint rc;

	SharedData() :
		rc(0)
	{
	}

	SharedData(const SharedData&) :
		rc(0)
	{
	}

	SharedData& operator=(const SharedData&) = delete;
};


template<typename T>
class SharedDataPointer {
public:
	static_assert(std::is_base_of<SharedData, T>::value,
			"T must be inherited from SharedData");
	
	using pointer = T*;
	using element_type = T;

	SharedDataPointer() :
		data_(nullptr)
	{
	}

	explicit SharedDataPointer(T* other) :
		data_(other)
	{
		if(data_)
			++data_->rc;
	}

	SharedDataPointer(const SharedDataPointer<T>& other) :
		data_(other.data_)
	{
		if(data_)
			++data_->rc;
	}

	~SharedDataPointer()
	{
		if(data_ && !--data_->rc)
			delete data_;
	}

	SharedDataPointer<T>& operator=(const SharedDataPointer<T>& other)
	{
		if(other.data_ != data_) {
			if(other.data_)
				++other.data_->rc;

			T* old = data_;
			data_ = other.data_;
			if(old && !--old->rc)
				delete old;
		}

		return *this;
	}

	SharedDataPointer<T>& operator=(T* other)
	{
		if(other != data_) {
			if(other)
				++other->rc;

			T* old = data_;
			data_ = other;
			if(old && !--old->rc)
				delete old;
		}

		return *this;
	}

	bool operator==(const SharedDataPointer<T>& other) const
	{
		return data_ == other.data_;
	}

	bool operator!=(const SharedDataPointer<T>& other) const
	{
		return data_ != other.data_;
	}

	bool operator==(const T* other) const
	{
		return data_ == other;
	}

	bool operator!=(const T* other) const
	{
		return data_ != other;
	}

	bool operator!() const
	{
		return !data_;
	}

	bool tryDetach()
	{
		if(data_ && data_->rc.load() != 1) {
			T* x = clone();
			++x->rc;
			if(!--data_->rc)
				delete data_;

			data_ = x;
			return true;
		}

		return false;
	}

	void detach()
	{
		T* x = clone();
		++x->rc;
		if(!--data_->rc)
			delete data_;

		data_ = x;
	}

	T& operator*()
	{
		return *data_;
	}

	const T& operator*() const
	{
		return *data_;
	}

	operator T*()
	{
		return data_;
	}

	operator const T*() const
	{
		return data_;
	}

	T* operator->()
	{
		return data_;
	}

	const T* operator->() const
	{
		return data_;
	}

	T* data()
	{
		return data_;
	}

	const T* data() const
	{
		return data_;
	}

	const T* constData() const
	{
		return data_;
	}

private:
	T* data_;

	T* clone()
	{
		return new T(*data_);
	}
};


} // namespace Tech


#endif // TECH_SHAREDDATA_H
