#ifndef TECH_TYPETRAITS_H
#define TECH_TYPETRAITS_H

#include <climits>
#include <limits>
#include <type_traits>


namespace internal {

enum class Enabler {};

}


namespace Tech {


template<typename T>
using RemoveReference = typename std::remove_reference<T>::type;

template<typename T>
using RemoveCv = typename std::remove_cv<T>::type;

template<typename T>
using Unqualified = RemoveCv<RemoveReference<T>>;

template<bool B, typename ...>
struct BoolConstType : std::integral_constant<bool, B> {};

template<bool B, typename ...T>
using BoolConst = typename BoolConstType<B, T...>::type;

template<typename If, typename Then, typename Else>
using Conditional = typename std::conditional<If::value, Then, Else>::type;

// Meta-logical negation
template <typename T>
using Not = BoolConst<!T::value>;

// Meta-logical conjunction
template<typename ...T>
struct All : BoolConst<true> {};

template<typename First, typename ...Others>
struct All<First, Others...> : Conditional<First, All<Others...>, BoolConst<false>> {};

// Meta-logical disjunction
template <typename... T>
struct Any : BoolConst<false> {};

template <typename First, typename... Others>
struct Any<First, Others...> : Conditional<First, BoolConst<true>, Any<Others...>> {};

template<typename ...Conds>
using EnableIf = typename std::enable_if<All<Conds...>::value, internal::Enabler>::type;

template<typename ...Conds>
using DisableIf = typename std::enable_if<!All<Conds...>::value, internal::Enabler>::type;


template<bool... b>
struct AllOf;

// recurse, if the first argument is true
template<bool... others>
struct AllOf<true, others...> : AllOf<others...> {};

// end recursion if first argument is false
template<bool... others>
struct AllOf<false, others...> : std::false_type {};

// no more arguments
template<>
struct AllOf<> : std::true_type {};


/**
 * Checks whether @c T is a boolean type.
 *
 * Provides the member constant @c value which is equal to @c true, if @c T is the type
 * bool. Otherwise, value is equal to @c false.
 */
template<typename T>
using IsBool = std::is_same<typename std::remove_cv<T>::type, bool>;


/**
 * Checks whether @c T is a integer type.
 *
 * Provides the member constant @c value which is equal to @c true, if @c T is type of
 * char, char16_t, char32_t, wchar_t, short, int, long, long long, or any
 * implementation-defined extended integer types, including any signed, unsigned, and
 * cv-qualified variants. Otherwise, @c value is equal to @c false.
 */
template<typename T>
struct IsInteger : std::integral_constant<bool,
		std::is_integral<T>::value &&
		!std::is_floating_point<T>::value &&
		!IsBool<T>::value> {
};


/**
 * Checks whether @c T can be dereferenced.
 *
 * Provides the member constant @c value which is equal to @c true, if @c T is the
 * pointer type, or any struct/class, which has dereference operator implementation.
 * Otherwise, @c value is equal to @c false.
 */
template<typename T>
class IsDereferenceable {
	typedef char yes;
	typedef long no;

	template<typename C>
	static yes test(decltype(&C::operator*));

	template<typename C>
	static no test(...);

public:
	static constexpr bool value = std::is_pointer<T>::value ||
			sizeof(test<T>(0)) == sizeof(yes);
};

template<typename T>
constexpr bool IsDereferenceable<T>::value;



template<typename F>
struct FunctionTraits;

template<typename R, typename ...Args>
struct FunctionTraits<R(*)(Args...)> : public FunctionTraits<R(Args...)> {};

template<typename R, typename ...Args>
struct FunctionTraits<R(Args...)> {
	using return_type = R;
	static constexpr std::size_t arity = sizeof...(Args);

	template<std::size_t N>
	struct argument {
		static_assert(N < arity, "index is out of range");
		using type = typename std::tuple_element<N,std::tuple<Args...>>::type;
	};
};


/**
 * Provides information about non-integer, non-floating point type limits.
 */
template<typename T, typename E = internal::Enabler>
struct Limits {
	/**
	 * Returns number of bits, required to represent the @c T type.
	 */
	static constexpr int bitCount()
	{
		return sizeof(T) * CHAR_BIT;
	}

	/**
	 * Returns number of bytes, required to represent the @c T type.
	 */
	static constexpr int size()
	{
		return bitCount() / 8;
	}
};


/**
 * Provides information about integer type limits.
 */
template<typename T>
struct Limits<T, EnableIf<std::is_integral<T>, Not<std::is_floating_point<T>>>> {
	/**
	 * Returns number of bits, required to represent the @c T type.
	 */
	static constexpr int bitCount()
	{
		return sizeof(T) * CHAR_BIT;
	}

	/**
	 * Returns number of bytes, required to represent the @c T type.
	 */
	static constexpr int size()
	{
		return bitCount() / 8;
	}

	/**
	 * Returns the minimum value that can be represented by type @c T.
	 */
	static constexpr T min()
	{
		return std::numeric_limits<T>::min();
	}

	/**
	 * Returns the maximum value that can be represented by type @c T.
	 */
	static constexpr T max()
	{
		return std::numeric_limits<T>::max();
	}

	/**
	 * Returns the number of bits not counting the sign bit for type @c T.
	 */
	static constexpr int significantBits()
	{
		return std::numeric_limits<T>::digits;
	}
};


/**
 * Provides information about floating point type limits.
 */
template<typename T>
struct Limits<T, EnableIf<std::is_floating_point<T>>> {
	/**
	 * Returns number of bits, required to represent the @c T type.
	 */
	static constexpr int bitCount()
	{
		return sizeof(T) * CHAR_BIT;
	}

	/**
	 * Returns number of bytes, required to represent the @c T type.
	 */
	static constexpr int size()
	{
		return bitCount() / 8;
	}

	/**
	 * Returns the minimum value that can be represented by type @c T.
	 */
	static constexpr T min()
	{
		return std::numeric_limits<T>::lowest();
	}

	/**
	 * Returns the maximum value that can be represented by type @c T.
	 */
	static constexpr T max()
	{
		return std::numeric_limits<T>::max();
	}

	/**
	 * Returns the smallest positive value that can be represented by type @c T.
	 */
	static constexpr T smallest()
	{
		return std::numeric_limits<T>::min();
	}

	/**
	 * Returns the number of decimal digits in the mantissa for the type @c T.
	 */
	static constexpr int mantissaSize()
	{
		return std::numeric_limits<T>::digits;
	}

	static constexpr T epsilon()
	{
		return std::numeric_limits<T>::epsilon();
	}
};


} // namespace Tech


#endif // TECH_TYPETRAITS_H
