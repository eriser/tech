#ifndef TECH_ITEMCODEC_H
#define TECH_ITEMCODEC_H

#include <queue>
#include <tech/bytearray.h>
#include <tech/item.h>
#include <tech/types.h>


namespace Tech {


class ItemCodec {
public:
	static ByteArray encode(const Item& item);
	static Item decode(const ByteArray& data, bool* isOk = nullptr);

private:
	enum Type {
		kNull,
		kBool,
		kSignedPositive,
		kSignedNegative,
		kUnsigned,
		kReal,
		kBinary,
		kString,
		kArray,
		kMap,
		kTag
	};

	static void encodeItem(const Item& item, ByteArray* output);
	static void writeValue(Type type, u64 value, ByteArray* output);

	static size_t decodeItem(const ByteArray& data, Item* output);
	static size_t readValue(const ByteArray& data, Type* type, u64* value);
};


} // namespace Tech


#endif // TECH_ITEMCODEC_H
