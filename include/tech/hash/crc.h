#ifndef TECH_HASH_CRC_H
#define TECH_HASH_CRC_H

#include <tech/traits.h>
#include <tech/types.h>
#include <tech/utils.h>


namespace Tech {


// "Стандартные" реализации CRC
u8 crc8(const void* data, size_t length);
u16 crc16(const void* data, size_t length);
u32 crc32(const void* data, size_t length);
u64 crc64(const void* data, size_t length);

u8 crc8Zero(const void* data, size_t length);

// Реализация CRC-16, используемая qChecksum() в библиотеке Qt
u16 crc16Ccitt(const void* data, size_t length);


template<typename T>
T* generateCrcTable(T polynom)
{
	static_assert(IsInteger<T>::value, "");

	T* table = new T[256];
	int bitSize = Limits<T>::bitCount();

	for(int i = 0; i < 256; ++i) {
		table[i] = static_cast<T>(i) << (bitSize - 8);

		for(int j = 0; j < 8; ++j) {
			if(table[i] & (static_cast<T>(0x80) << (bitSize - 8))) {
				table[i] = (table[i] << 1) ^ polynom;
			}
			else {
				table[i] = table[i] << 1;
			}
		}
	}

	return table;
}


template<typename T>
T* generateReverseCrcTable(T polynom)
{
	static_assert(IsInteger<T>::value, "");

	T* table = new T[256];
	polynom = reverseBits<T>(polynom);

	for(int i = 0; i < 256; ++i) {
		table[i] = i;

		for(int j = 0; j < 8; ++j) {
			if(table[i] & 0x01) {
				table[i] = (table[i] >> 1) ^ polynom;
			}
			else {
				table[i] = table[i] >> 1;
			}
		}
	}

	return table;
}


template<typename T>
T calculateCrc(const void* data, size_t length, const T* table, T initialValue)
{
	static_assert(IsInteger<T>::value, "");

	const u8 *buffer = static_cast<const u8*>(data);
	T result = initialValue;
	int bitSize = Limits<T>::bitCount();

	while(length--) {
		// index = MSB xor CURRENT
		int index = (result >> (bitSize - 8)) ^ *buffer++;
		result = (result << 8) ^ table[index];
	}

	return result;
}


template<typename T>
T calculateReverseCrc(const void* data, size_t length, const T* table, T initialValue)
{
	static_assert(IsInteger<T>::value, "");

	const u8* buffer = static_cast<const u8*>(data);
	T result = initialValue;

	while(length--) {
		// index = LSB xor CURRENT
		int index = (result & 0xFF) ^ *buffer++;
		result = (result >> 8) ^ table[index];
	}

	return result;
}


} // namespace Tech


#endif // TECH_HASH_CRC_H
