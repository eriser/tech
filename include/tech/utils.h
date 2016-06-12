#ifndef TECH_UTILS_H
#define TECH_UTILS_H

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <tech/platform.h>
#include <tech/traits.h>
#include <tech/types.h>


#define UNUSED(x) (static_cast<void>(x))

#define MAKE_MAGIC(a, b, c, d) (a << 24 | b << 16 | c << 8 | d)

#define __FILENAME__ ({                                         \
		static const int index = Tech::basenameIndex(__FILE__); \
		static_assert(index >= 0, "compile-time basename");     \
        __FILE__ + index; })

#if defined(DEBUG)
#define ASSERT(expression, message) \
		do { \
			if(!(expression)) { \
				std::cerr << __FILENAME__ << ':' << __LINE__ << message << std::endl; \
				std::abort(); \
			} \
		} while(0)
#else
#define ASSERT(expression, message)
#endif


namespace Tech {


template<typename T>
constexpr T kPi = T(3.1415926535897932385);


#ifndef PLATFORM_WINDOWS
static const char kPathDelimiter = '/';
#else
static const char kPathDelimiter = '\\';
#endif // PLATFORM_WINDOWS


/**
 * Returns the index of the last path delimiter + 1 (an index of file's base name).
 * Effect of this function should be computed at compile time.
 */
static constexpr
int basenameIndex(const char* const path, const int index = 0, const int slashIndex = -1)
{
	return path[index] ? (path[index] == kPathDelimiter ?
						  basenameIndex(path, index + 1, index) :
						  basenameIndex(path, index + 1, slashIndex)) : (slashIndex + 1);
}


/**
 * Returns the minimum of @p a and @p b.
 */
template<typename T>
constexpr const T& minOf(const T& a, const T& b)
{
	return a < b ? a : b;
}


/**
 * Returns the minimum value of all arguments.
 */
template<typename T, typename ...Rest>
constexpr const T& minOf(const T& first, const Rest&... rest)
{
	return minOf(first, minOf(rest...));
}


/**
 * Returns the maximum of @p a and @p b.
 */
template<typename T>
constexpr const T& maxOf(const T& a, const T& b)
{
	return a > b ? a : b;
}


/**
 * Returns the maximum value of all arguments.
 */
template<typename T, typename ...Rest>
constexpr const T& maxOf(const T& first, const Rest&... rest)
{
	return maxOf(first, maxOf(rest...));
}


/**
 * Returns @c value bounded by @p minimum and @p maximum.
 *
 * This is equivalent to max(minimum, min(value, maximum)).
 */
template<typename T>
constexpr const T& bound(const T& minimum, const T& value, const T& maximum)
{
	return maxOf(minimum, minOf(value, maximum));
}


/**
 * Returns whole part of the floating point @p value.
 */
static inline
float whole(float value)
{
	return static_cast<i32>(value);
}


/**
 * Returns whole part of the floating point @p value.
 */
static inline
double whole(double value)
{
	return static_cast<i64>(value);
}


/**
 * Returns fraction part of the floating point @p value.
 */
static inline
float fraction(float value)
{
	return value - whole(value);
}


/**
 * Returns fraction part of the floating point @p value.
 */
static inline
double fraction(double value)
{
	return value - whole(value);
}


/**
 * Returns @p value, rounded to the nearest integer.
 */
template<typename T>
constexpr T round(T value)
{
	static_assert(std::is_floating_point<T>::value, "T must be a floating point value");

	if(value > 0.0)
		return whole(value + 0.5);

	return whole(value - 0.5);
}


template<typename T>
constexpr T fuzzyIsNull(T value)
{
	static_assert(std::is_floating_point<T>::value, "T must be a floating point value");

	return std::abs(value) <= Limits<T>::epsilon();
}


template<typename T>
constexpr T fuzzyIsEqual(T value1, T value2)
{
	static_assert(std::is_floating_point<T>::value, "T must be a floating point value");

	if(value1 < value2)
		return value2 - value1 <= Limits<T>::epsilon() * std::abs(value1);

	return value1 - value2 <= Limits<T>::epsilon() * std::abs(value2);
}


static inline
int countLeadingZeroesHelper32(u32 value)
{
	static const u8 kTable[] = {
		32, 31, 30, 30, 29, 29, 29, 29,	28, 28, 28, 28, 28, 28, 28, 28,
		27, 27, 27, 27, 27, 27, 27, 27,	27, 27, 27, 27, 27, 27, 27, 27,
		26, 26, 26, 26, 26, 26, 26, 26,	26, 26, 26, 26, 26, 26, 26, 26,
		26, 26, 26, 26, 26, 26, 26, 26,	26, 26, 26, 26, 26, 26, 26, 26,
		25, 25, 25, 25, 25, 25, 25, 25,	25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,	25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,	25, 25, 25, 25, 25, 25, 25, 25,
		25, 25, 25, 25, 25, 25, 25, 25,	25, 25, 25, 25, 25, 25, 25, 25,
		24, 24, 24, 24, 24, 24, 24, 24,	24, 24, 24, 24, 24, 24, 24, 24,
		24, 24, 24, 24, 24, 24, 24, 24,	24, 24, 24, 24, 24, 24, 24, 24,
		24, 24, 24, 24, 24, 24, 24, 24,	24, 24, 24, 24, 24, 24, 24, 24,
		24, 24, 24, 24, 24, 24, 24, 24,	24, 24, 24, 24, 24, 24, 24, 24,
		24, 24, 24, 24, 24, 24, 24, 24,	24, 24, 24, 24, 24, 24, 24, 24,
		24, 24, 24, 24, 24, 24, 24, 24,	24, 24, 24, 24, 24, 24, 24, 24,
		24, 24, 24, 24, 24, 24, 24, 24,	24, 24, 24, 24, 24, 24, 24, 24,
		24, 24, 24, 24, 24, 24, 24, 24,	24, 24, 24, 24, 24, 24, 24, 24
	};

	u32 n;

	if(value >= (1 << 16)) {
		if(value >= (1 << 24)) {
			n = 24;
		}
		else {
			n = 16;
		}
	}
	else {
		if(value >= (1 << 8)) {
			n = 8;
		}
		else {
			n = 0;
		}
	}

	return kTable[value >> n] - n;
}


static inline
int countLeadingZeroesHelper64(u64 value)
{
	if(value < 0x100000000)
		return countLeadingZeroesHelper32(value) + 32;

	return countLeadingZeroesHelper32(value >> 32);
}


/**
 * Returns the number of leading zero bits in @p value.
 */
template<typename T>
int countLeadingZeroes(T value)
{
	static_assert(std::is_integral<T>::value, "T must be an integral value");

	if(value == 0)
		return Limits<T>::bitCount();

	const int kDiff = Limits<uint>::bitCount() - Limits<T>::bitCount();

#if defined(__GNUG__) || defined(__clang__)
	return __builtin_clz(value) - kDiff;
#else
	return countLeadingZeroesHelper32(value) - kDiff;
#endif
}


template<> inline
int countLeadingZeroes(uint value)
{
	if(value == 0)
		return Limits<uint>::bitCount();

#if defined(__GNUG__) || defined(__clang__)
	return __builtin_clz(value);
#else
	if(Limits<uint>::bitCount() == 64)
		return countLeadingZeroesHelper64(value);

	return countLeadingZeroesHelper32(value);
#endif
}


template<> inline
int countLeadingZeroes(int value)
{
	return countLeadingZeroes(static_cast<uint>(value));
}


template<> inline
int countLeadingZeroes(ulong value)
{
	if(value == 0)
		return Limits<ulong>::bitCount();

#if defined(__GNUG__) || defined(__clang__)
	return __builtin_clzl(value);
#else
	if(Limits<ulong>::bitCount() == 64)
		return countLeadingZeroesHelper64(value);

	return countLeadingZeroesHelper32(value);
#endif
}


template<> inline
int countLeadingZeroes(long value)
{
	return countLeadingZeroes(static_cast<ulong>(value));
}


template<> inline
int countLeadingZeroes(ulonglong value)
{
	if(value == 0)
		return Limits<ulonglong>::bitCount();

#if defined(__GNUG__) || defined(__clang__)
	return __builtin_clzll(value);
#else
	if(Limits<ulonglong>::bitCount() == 64)
		return countLeadingZeroesHelper64(value);

	return countLeadingZeroesHelper32(value);
#endif
}


template<> inline
int countLeadingZeroes(longlong value)
{
	return countLeadingZeroes(static_cast<ulonglong>(value));
}


/**
 * Returns the most significant bit in @p value or -1 if all bits are zero.
 *
 * @pre T must be an integral type.
 */
template<typename T>
int mostSignificantBit(T value)
{
	static_assert(std::is_integral<T>::value, "T must be an integral value");

	return Limits<T>::bitCount() - countLeadingZeroes(value) - 1;
}


static inline
int countSetBitsHelper32(u32 value)
{
	value = value - ((value >> 1) & 0x55555555);
	value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
	value = ((value + (value >> 4)) & 0x0F0F0F0F);
	return (value * 0x01010101) >> 24;
}


static inline
int countSetBitsHelper64(u64 value)
{
	value = value - ((value >> 1) & 0x5555555555555555);
	value = (value & 0x3333333333333333) + ((value >> 2) & 0x3333333333333333);
	value = ((value + (value >> 4)) & 0x0F0F0F0F0F0F0F0F);
	return int((value * 0x0101010101010101) >> 56);
}


/**
 * Returns the number of set bits in @p value.
 */
template<typename T>
int countSetBits(T value)
{
	static_assert(std::is_integral<T>::value, "T must be an integral value");

#if defined(__GNUG__) || defined(__clang__)
	return __builtin_popcount(value);
#else
	return countSetBitsHelper32(value);
#endif
}


template<> inline
int countSetBits(ulong value)
{
#if defined(__GNUG__) || defined(__clang__)
	return __builtin_popcountl(value);
#else
	if(Limits<ulong>::bitCount() == 64)
		return countSetBitsHelper64(value);

	return countSetBitsHelper32(value);
#endif
}


template<> inline
int countSetBits(long value)
{
	return countSetBits(static_cast<ulong>(value));
}


template<> inline
int countSetBits(ulonglong value)
{
#if defined(__GNUG__) || defined(__clang__)
	return __builtin_popcountll(value);
#else
	if(Limits<ulonglong>::bitCount() == 64)
		return countSetBitsHelper64(value);

	return countSetBitsHelper32(value);
#endif
}


template<> inline
int countSetBits(longlong value)
{
	return countSetBits(static_cast<ulonglong>(value));
}


/**
 * Returns the number of reset bits in @p value.
 *
 * @pre T must be an integral type.
 */
template<typename T>
int countResetBits(T value)
{
	static_assert(std::is_integral<T>::value, "T must be an integral type");

	return Limits<T>::bitCount() - countSetBits(value);
}


/**
 * Rotates bits of the @p value to left by @p n positions and returns the result. If
 * @p n is greater than Limits<T>::bitCount(), then result will be undefined.
 *
 * @pre T must be an integral type.
 */
template<typename T>
T rotateBitsLeft(T value, uint n)
{
	static_assert(IsInteger<T>::value, "T must be an integer type");
	static_assert(std::is_unsigned<T>::value, "T must be an unsigned type");

	n &= Limits<T>::bitCount() - 1;
	return (value << n) | (value >> (Limits<T>::bitCount() - n));
}


/**
 * Rotates bits of the @p value to right by @p n positions and returns the result. If
 * @p n is greater than Limits<T>::bitCount(), then result will be undefined.
 *
 * @pre T must be an integral type.
 */
template<typename T>
T rotateBitsRight(T value, uint n)
{
	static_assert(IsInteger<T>::value, "T must be an integer type");
	static_assert(std::is_unsigned<T>::value, "T must be an unsigned type");

	n &= Limits<T>::bitCount() - 1;
	return (value >> n) | (value << (Limits<T>::bitCount() - n));
}


/**
 * Rotates bits of the @p value by @p n positions and returns the result. If @n is less
 * than zero, the rotation is performed to the left side. If absolute value of @p n is
 * greater than Limits<T>::bitCount(), then result will be undefined.
 *
 * @pre T must be an integral type.
 */
template<typename T>
T rotateBits(T value, int n)
{
	static_assert(IsInteger<T>::value, "T must be an integer type");
	static_assert(std::is_unsigned<T>::value, "T must be an unsigned type");

	if(n < 0) {
		return rotateBitsLeft(value, -n);
	}
	else if(n > 0) {
		return rotateBitsRight(value, n);
	}

	return value;
}


/**
 * Returns the most significant decimal digit in @p value or -1 if all digits are zero.
 *
 * @pre T must be an integral type.
 */
template<typename T>
int mostSignificantDigit(T value)
{
	static_assert(std::is_integral<T>::value, "T must be an integral type");

	static const u64 powersOf10[] = {
		1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000,
		10000000000u, 100000000000u, 1000000000000u, 10000000000000u, 100000000000000u,
		1000000000000000u, 10000000000000000u, 100000000000000000u, 1000000000000000000u,
		10000000000000000000u
	};

	u64 temp = (mostSignificantBit(value) + 1) * 1233 >> 12;
	return temp - (value < powersOf10[temp]);
}


/**
 * Reverse bits of the @p value from right to left and returns the result.
 *
 * @pre T should be an integral type.
 */
template<typename T>
T reverseBits(T value)
{
	static_assert(std::is_integral<T>::value, "T must be an integral type");

	static const u8 bitReverseTable[256] = {
		0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0,
		0x30, 0xB0, 0x70, 0xF0, 0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
		0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 0x04, 0x84, 0x44, 0xC4,
		0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
		0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC,
		0x3C, 0xBC, 0x7C, 0xFC, 0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
		0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 0x0A, 0x8A, 0x4A, 0xCA,
		0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
		0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6,
		0x36, 0xB6, 0x76, 0xF6, 0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
		0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,	0x01, 0x81, 0x41, 0xC1,
		0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
		0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9,
		0x39, 0xB9, 0x79, 0xF9, 0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
		0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5, 0x0D, 0x8D, 0x4D, 0xCD,
		0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
		0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3,
		0x33, 0xB3, 0x73, 0xF3, 0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
		0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,	0x07, 0x87, 0x47, 0xC7,
		0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
		0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF,
		0x3F, 0xBF, 0x7F, 0xFF
	};

	T result;
	u8* source = reinterpret_cast<u8*>(&value);
	u8* dest = reinterpret_cast<u8*>(&result);

	int byteCount = Limits<T>::size();
	for(int i = 0; i < byteCount; ++i)
		dest[byteCount - i - 1] = bitReverseTable[source[i]];

	return result;
}


static inline
u16 swapBytesHelper16(u16 value)
{
	return (value << 8) | (value >> 8);
}


static inline
u32 swapBytesHelper32(u32 value)
{
	value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0x00FF00FF);
	return (value << 16) | (value >> 16);
}


static inline
u64 swapBytesHelper64(u64 value)
{
	value = ((value << 8) & 0xFF00FF00FF00FF00ull)
			| ((value >> 8) & 0x00FF00FF00FF00FFull);

	value = ((value << 16) & 0xFFFF0000FFFF0000ull)
			| ((value >> 16) & 0x0000FFFF0000FFFFull);

	return (value << 32) | (value >> 32);
}


/**
 * Swaps bytes of the @p value from right to left and returns the result.
 */
template<typename T>
T swapBytes(T value);


template<> inline
u16 swapBytes(u16 value)
{
	return swapBytesHelper16(value);
}


template<> inline
i16 swapBytes(i16 value)
{
	return swapBytesHelper16(value);
}


template<> inline
u32 swapBytes(u32 value)
{
#if defined(__GNUG__) || defined(__clang__)
	return __builtin_bswap32(value);
#else
	return swapBytesHelper32(value);
#endif
}


template<> inline
i32 swapBytes(i32 value)
{
	return swapBytesHelper32(value);
}


template<> inline
u64 swapBytes(u64 value)
{
#if defined(__GNUG__) || defined(__clang__)
	return __builtin_bswap64(value);
#else
	return swapBytesHelper64(value);
#endif
}


template<> inline
i64 swapBytes(i64 value)
{
	return swapBytesHelper64(value);
}


/**
 * Converts @p value from native to big endian byte order and return the result.
 *
 * @pre T must be an integral type.
 */
template<typename T>
T toBigEndian(T value)
{
	static_assert(std::is_integral<T>::value, "T must be an integral type");

#ifdef PLATFORM_BIG_ENDIAN
	return value;
#else
	return swapBytes(value);
#endif
}


/**
 * Converts @p value from native to little endian byte order and return the result.
 *
 * @pre T must be an integral type.
 */
template<typename T>
T toLittleEndian(T value)
{
	static_assert(std::is_integral<T>::value, "T must be an integral type");

#ifdef PLATFORM_BIG_ENDIAN
	return swapBytes(value);
#else
	return value;
#endif
}


/**
 * Converts @p value from big to native endian byte order and return the result.
 *
 * @pre T must be an integral type.
 */
template<typename T>
T fromBigEndian(T value)
{
	static_assert(std::is_integral<T>::value, "T must be an integral type");

#ifdef PLATFORM_BIG_ENDIAN
	return value;
#else
	return swapBytes(value);
#endif
}


/**
 * Converts @p value from little to native endian byte order and return the result.
 *
 * @pre T must be an integral type.
 */
template<typename T>
T fromLittleEndian(T value)
{
	static_assert(std::is_integral<T>::value, "T must be an integral type");

#ifdef PLATFORM_BIG_ENDIAN
	return swapBytes(value);
#else
	return value;
#endif
}


/**
 * Returns @p count number of high bits from @p value.
 *
 * @pre T must be an integral type.
 */
template<typename T>
T highBits(T value, int count = Limits<T>::bitCount() / 2)
{
	static_assert(std::is_integral<T>::value, "T must be an integral type");

	if(count <= 0)
		return 0;

	return value >> (Limits<T>::bitCount() - count);
}


/**
 * Returns @p count number of low bits from @p value.
 *
 * @pre T must be an integral type.
 */
template<typename T>
T lowBits(T value, int count = Limits<T>::bitCount() / 2)
{
	static_assert(std::is_integral<T>::value, "T must be an integral type");

	if(count <= 0)
		return 0;

	int shift = Limits<T>::bitCount() - count;
	return (value << shift) >> shift;
}


/**
 * Returns @c true if the @p value is a power of two.
 *
 * @pre T must be an integer type.
 */
template<typename T>
bool isPowerOfTwo(T value)
{
	static_assert(std::is_integral<T>::value, "T must be an integral type");

	return value && !(value & (value - 1));
}


/**
 * Returns nearest power of two, that greater or equal to @p value.
 */
template<typename T>
T ceilToPowerOfTwo(T value);


template<> inline
u16 ceilToPowerOfTwo(u16 value)
{
	if(value != 0) {
		value--;

		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
	}

	return value + 1;
}


template<> inline
i16 ceilToPowerOfTwo(i16 value)
{
	return ceilToPowerOfTwo(static_cast<u16>(value));
}


template<> inline
u32 ceilToPowerOfTwo(u32 value)
{
#if defined(__GNUG__) || defined(__clang__)
	if(value <= 2)
		return (value >> 1) + 1;

	return 1 << (32 - __builtin_clz(value - 1));
#else
	if(value != 0) {
		value--;

		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;
	}

	return value + 1;
#endif
}


template<> inline
i32 ceilToPowerOfTwo(i32 value)
{
	return ceilToPowerOfTwo(static_cast<u32>(value));
}


template<> inline
u64 ceilToPowerOfTwo(u64 value)
{
#if defined(__GNUG__) || defined(__clang__)
	if(value <= 2)
		return (value >> 1) + 1;

	return 1ull << (64 - __builtin_clzll(value - 1));
#else
	if(value != 0) {
		value--;

		value |= value >> 1;
		value |= value >> 2;
		value |= value >> 4;
		value |= value >> 8;
		value |= value >> 16;
		value |= value >> 32;
	}

	return value + 1;
#endif
}


template<> inline
i64 ceilToPowerOfTwo(i64 value)
{
	return ceilToPowerOfTwo(static_cast<u64>(value));
}


/**
 * Returns nearest power of two, that less or equal to @p value.
 */
template<typename T>
T floorToPowerOfTwo(T value);


template<> inline
u16 floorToPowerOfTwo(u16 value)
{
	return ceilToPowerOfTwo(value + 1) >> 1;
}


template<> inline
i16 floorToPowerOfTwo(i16 value)
{
	return ceilToPowerOfTwo(static_cast<u16>(value) + 1) >> 1;
}


template<> inline
u32 floorToPowerOfTwo(u32 value)
{
	return ceilToPowerOfTwo(value + 1) >> 1;
}


template<> inline
i32 floorToPowerOfTwo(i32 value)
{
	return ceilToPowerOfTwo(static_cast<u32>(value) + 1) >> 1;
}


template<> inline
u64 floorToPowerOfTwo(u64 value)
{
	return ceilToPowerOfTwo(value + 1) >> 1;
}


template<> inline
i64 floorToPowerOfTwo(i64 value)
{
	return ceilToPowerOfTwo(static_cast<u64>(value) + 1) >> 1;
}


} // namespace


#endif // TECH_UTILS_H
