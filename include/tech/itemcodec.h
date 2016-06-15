#ifndef TECH_ITEMCODEC_H
#define TECH_ITEMCODEC_H

#include <queue>
#include <tech/binary.h>
#include <tech/item.h>
#include <tech/types.h>


namespace Tech {


class ItemCodec {
public:
	static Binary encode(const Item& item);
	static Item decode(const Binary& data, bool* isOk = nullptr);

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

	static void encodeItem(const Item& item, Binary* output);
	static void writeValue(Type type, u64 value, Binary* output);

	static size_t decodeItem(const Binary& data, Item* output);
	static size_t readValue(const Binary& data, Type* type, u64* value);
};


} // namespace Tech


#endif // TECH_ITEMCODEC_H
