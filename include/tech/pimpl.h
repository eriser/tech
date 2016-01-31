#ifndef TECH_PIMPL_H
#define TECH_PIMPL_H

#include <tech/types.h>
#include <tech/traits.h>


namespace Tech {


/**
 * This is a helper class for creating public class of a pimpl (an interface).
 * T - type of embedded pointer to Implementation
 * B - type of base class (should be omitted for primary class)
 *
 * Example:
 *   class SerialDevice : public Interface<SerialDeviceImpl> { };
 *   class File : public Interface<FileImpl, SerialDevice> { };
 */
template<typename T, typename B = void, typename = internal::Enabler>
class Interface;


template<typename T>
class Interface<T, void> {
public:
	using Type = T;

	Interface(T* impl) :
		impl_(impl)
	{
	}

	Interface(Arc<T> impl) :
		impl_(impl)
	{
	}

	virtual ~Interface() = default;

protected:
	T* impl() const
	{
		return impl_.get();
	}

private:
	Arc<T> impl_;
};


template<typename T, typename B>
class Interface<T, B, EnableIf<
		std::is_base_of<Interface<typename B::Type, void>, B>>> : public B {
public:
	Interface(T* impl) :
		B(impl)
	{
	}

protected:
	T* impl() const
	{
		return static_cast<T*>(B::impl());
	}
};


/**
 * This is a helper class for creating private class of a pimpl (an implementation).
 * T - type of embedded pointer to Interface
 * B - type of base class (should be omitted for primary class)
 *
 * Example:
 *   class SerialDeviceImpl : public Implementation<SerialDevice> { };
 *   class FileImpl : public Implementation<File, SerialDeviceImpl> { };
 */
template<typename T, typename B = void, typename = internal::Enabler>
class Implementation;


template<typename T>
class Implementation<T, void> {
public:
	using Type = T;

	Implementation(T* iface) :
		iface_(iface)
	{
	}

	virtual ~Implementation() = default;

protected:
	T* iface() const
	{
		return iface_;
	}

private:
	T* iface_;
};


template<typename T, typename B>
class Implementation<T, B, EnableIf<
		std::is_base_of<Implementation<typename B::Type, void>, B>>> : public B {
public:
	Implementation(T* q) :
		B(q)
	{
	}

protected:
	T* iface() const
	{
		return static_cast<T*>(B::q());
	}
};


} // namespace Tech


#endif // TECH_PIMPL_H
