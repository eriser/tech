#ifndef TECH_FLAGS_H
#define TECH_FLAGS_H

#include <type_traits>


#define DECLARE_FLAG_OPERATORS(E)                                         \
	static_assert(std::is_enum<E>::value, "E must be an enum type");      \
																		  \
	inline constexpr Tech::Flags<E> operator&(E e1, E e2)                 \
	{                                                                     \
		return Tech::Flags<E>(e1) & Tech::Flags<E>(e2);                   \
	}                                                                     \
																		  \
	inline constexpr Tech::Flags<E> operator&(E e, Tech::Flags<E> f)      \
	{                                                                     \
		return f & Tech::Flags<E>(e);                                     \
	}                                                                     \
																		  \
	inline constexpr Tech::Flags<E> operator|(E e1, E e2)                 \
	{                                                                     \
		return Tech::Flags<E>(e1) | Tech::Flags<E>(e2);                   \
	}                                                                     \
																		  \
	inline constexpr Tech::Flags<E> operator|(E e, Tech::Flags<E> f)      \
	{                                                                     \
		return f | Tech::Flags<E>(e);                                     \
	}                                                                     \
																		  \
	inline constexpr Tech::Flags<E> operator^(E e1, E e2)                 \
	{                                                                     \
		return Tech::Flags<E>(e1) ^ Tech::Flags<E>(e2);                   \
	}                                                                     \
																		  \
	inline constexpr Tech::Flags<E> operator^(E e, Tech::Flags<E> f)      \
	{                                                                     \
		return f ^ Tech::Flags<E>(e);                                     \
	}


namespace Tech {


template<typename T>
class Flags {
	static_assert(std::is_enum<T>::value, "T must be an enum type");

	using U = typename std::underlying_type<T>::type;

public:
	using EnumType = T;

	constexpr Flags();
	constexpr Flags(const Flags& other);
	constexpr Flags(T flag);

	constexpr operator bool() const;

	constexpr bool operator!() const;

	constexpr Flags operator~() const;

	constexpr bool operator==(T flag) const;
	constexpr bool operator!=(T flag) const;

	constexpr Flags operator&(Flags mask) const;
	constexpr Flags operator&(T mask) const;

	Flags& operator&=(Flags mask);
	Flags& operator&=(T mask);

	constexpr Flags operator|(Flags mask) const;
	constexpr Flags operator|(T mask) const;

	Flags& operator|=(Flags mask);
	Flags& operator|=(T mask);

	constexpr Flags operator^(Flags mask) const;
	constexpr Flags operator^(T mask) const;

	Flags& operator^=(Flags mask);
	Flags& operator^=(T mask);

	constexpr U rawValue() const;

private:
	U flags_;
};


template<typename T>
constexpr Flags<T>::Flags() :
	flags_(U())
{
}


template<typename T>
constexpr Flags<T>::Flags(const Flags<T>& other) :
	flags_(other.flags_)
{
}


template<typename T>
constexpr Flags<T>::Flags(T flag) :
	flags_(static_cast<U>(flag))
{
}


template<typename T>
constexpr Flags<T>::operator bool() const
{
	return flags_ != U();
}


template<typename T>
constexpr bool Flags<T>::operator!() const
{
	return flags_ == U();
}


template<typename T>
constexpr Flags<T> Flags<T>::operator~() const
{
	Flags<T> result;
	result.flags_ = ~flags_;
	return result;
}


template<typename T>
constexpr bool Flags<T>::operator==(T flag) const
{
	return flags_ == static_cast<Flags<T>::U>(flag);
}


template<typename T>
constexpr bool Flags<T>::operator!=(T flag) const
{
	return flags_ != static_cast<Flags<T>::U>(flag);
}


template<typename T>
constexpr Flags<T> Flags<T>::operator&(Flags<T> mask) const
{
	Flags<T> result = *this;
	result &= mask;
	return result;
}


template<typename T>
constexpr Flags<T> Flags<T>::operator&(T mask) const
{
	Flags<T> result = *this;
	result &= mask;
	return result;
}


template<typename T>
Flags<T>& Flags<T>::operator&=(Flags<T> mask)
{
	flags_ &= mask.flags_;
	return *this;
}


template<typename T>
Flags<T>& Flags<T>::operator&=(T mask)
{
	flags_ &= static_cast<U>(mask);
	return *this;
}


template<typename T>
constexpr Flags<T> Flags<T>::operator|(Flags<T> mask) const
{
	Flags<T> result = *this;
	result |= mask;
	return result;
}


template<typename T>
constexpr Flags<T> Flags<T>::operator|(T mask) const
{
	Flags<T> result = *this;
	result |= mask;
	return result;
}


template<typename T>
Flags<T>& Flags<T>::operator|=(Flags<T> mask)
{
	flags_ |= mask.flags_;
	return *this;
}


template<typename T>
Flags<T>& Flags<T>::operator|=(T mask)
{
	flags_ |= static_cast<U>(mask);
	return *this;
}


template<typename T>
constexpr Flags<T> Flags<T>::operator^(Flags<T> mask) const
{
	Flags<T> result = *this;
	result ^= mask;
	return result;
}


template<typename T>
constexpr Flags<T> Flags<T>::operator^(T mask) const
{
	Flags<T> result = *this;
	result ^= mask;
	return result;
}


template<typename T>
Flags<T>& Flags<T>::operator^=(Flags<T> mask)
{
	flags_ ^= mask.flags_;
	return *this;
}


template<typename T>
Flags<T>& Flags<T>::operator^=(T mask)
{
	flags_ ^= static_cast<U>(mask);
	return *this;
}


template<typename T>
constexpr typename Flags<T>::U Flags<T>::rawValue() const
{
	return flags_;
}


} // namespace Tech


#endif // TECH_FLAGS_H
