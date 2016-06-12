#include <tech/itemcodec.h>


namespace Tech {


ByteArray ItemCodec::encode(const Item& item)
{
	ByteArray result;
	encodeItem(item, &result);
	return result;
}


Item ItemCodec::decode(const ByteArray& data, bool* isOk)
{
	Item result;
	if(!decodeItem(data, &result)) {
		if(isOk)
			*isOk = false;

		return Item();
	}

	if(isOk)
		*isOk = true;

	return result;
}


void ItemCodec::encodeItem(const Item& item, ByteArray* output)
{
	if(item.hasTag())
		writeValue(kTag, item.tag(), output);

	switch(item.type()) {
	case ItemType::kNull:
		writeValue(kNull, 0, output);
		break;

	case ItemType::kBool:
		writeValue(kBool, 0, output);
		break;

	case ItemType::kInteger: {
		i64 value = item.toInteger();
		if(value < 0) {
			value = -1 - value;
			writeValue(kSignedNegative, static_cast<u64>(value), output);
		}
		else {
			writeValue(kSignedPositive, static_cast<u64>(value), output);
		}

		break; }

	case ItemType::kUnsigned:
		writeValue(kUnsigned, item.toUnsigned(), output);
		break;

	case ItemType::kReal: {
		union {
			double real;
			u64 raw;
		};

		real = item.toReal();
		writeValue(kReal, raw, output);
		break; }

	case ItemType::kBinary:
		writeValue(kBinary, item.count(), output);
		*output += item.toBinary();
		break;

	case ItemType::kString: {
		ByteArray utf8 = item.toString().toUtf8();
		writeValue(kString, utf8.length(), output);
		*output += utf8;
		break; }

	case ItemType::kArray: {
		uint count = item.count();
		writeValue(kArray, count, output);

		for(uint i = 0; i < count; ++i)
			encodeItem(item[i], output);

		break; }

	case ItemType::kMap: {
		uint count = item.count();
		writeValue(kMap, count, output);

		for(uint i = 0; i < count; ++i) {
			Item::Field field = item.field(i);
			encodeItem(field.first, output);
			encodeItem(field.second, output);
		}

		break; }
	}
}


void ItemCodec::writeValue(Type type, u64 value, ByteArray* output)
{
	if((value >> 8) == 0) {
		if(value < 12) {
			*output += type << 4 | value;
		}
		else {
			*output += type << 4 | 12;
			*output += value;
		}
	}
	else if((value >> 16) == 0) {
		*output += type << 4 | 13;
		*output += value >> 8;
		*output += value;
	}
	else if(value >> 32 == 0) {
		*output += type << 4 | 14;
		*output += value >> 24;
		*output += value >> 16;
		*output += value >> 8;
		*output += value;
	}
	else {
		*output += type << 4 | 15;
		*output += value >> 56;
		*output += value >> 48;
		*output += value >> 40;
		*output += value >> 32;
		*output += value >> 24;
		*output += value >> 16;
		*output += value >> 8;
		*output += value;
	}
}


size_t ItemCodec::decodeItem(const ByteArray& data, Item* output)
{
	Type type;
	u64 value;
	u32 tag = 0;

	size_t count;
	size_t pos = readValue(data, &type, &value);
	if(!pos)
		return 0;

	if(type == kTag) {
		tag = static_cast<u32>(value);
		count = readValue(data.middleRef(pos), &type, &value);
		if(!count)
			return 0;

		pos += count;
	}

	switch(type) {
	case kTag:
		// Tag can't follow another tag
		return 0;

	case kNull:
		if(value != 0)
			return 0;

		break;

	case kBool: {
		if(value > 1)
			return 0;

		*output = static_cast<bool>(value);
		break; }

	case kSignedPositive: {
		*output = static_cast<i64>(value);
		break; }

	case kSignedNegative: {
		*output = static_cast<i64>(-1 - value);
		break; }

	case kUnsigned:
		*output = value;
		break;

	case kReal: {
		union {
			double real;
			u64 raw;
		};

		raw = value;
		*output = real;
		break; }

	case kBinary: {
		if(data.length() < pos + value)
			return 0;

		*output = data.middle(pos, value);
		pos += value;
		break; }

	case kString: {
		if(data.length() < pos + value)
			return 0;

		*output = String(data.middle(pos, value));
		pos += value;
		break; }

	case kArray: {
		for(uint i = 0; i < value; ++i) {
			count = decodeItem(data.middleRef(pos), &(*output)[i]);
			if(!count)
				return 0;

			pos += count;
		}

		break; }

	case kMap: {
		for(uint i = 0; i < value; ++i) {
			Item name;
			count = decodeItem(data.middleRef(pos), &name);
			if(!count)
				return 0;

			pos += count;

			Item value;
			count = decodeItem(data.middleRef(pos), &value);
			if(!count)
				return 0;

			pos += count;
			(*output)[name.toString().toUtf8()] = value;
		}
		break; }

	default:
		// An unknown type found
		return 0;
	}

	if(tag > 0)
		output->setTag(tag);

	return pos;
}


size_t ItemCodec::readValue(const ByteArray& data, Type* type, u64* value)
{
	if(data.isEmpty())
		return 0;

	size_t i = 0;
	const u8* ptr = reinterpret_cast<const u8*>(data.data());
	*type = static_cast<Type>(ptr[i] >> 4);
	u8 extra = static_cast<u8>(ptr[i++] & 0x0F);

	if(extra > 11 && data.length() < (1u << (extra - 12)))
		return 0;

	*value = 0;

	switch(extra) {
	case 15:
		*value |= static_cast<u64>(ptr[i++]) << 56;
		*value |= static_cast<u64>(ptr[i++]) << 48;
		*value |= static_cast<u64>(ptr[i++]) << 40;
		*value |= static_cast<u64>(ptr[i++]) << 32;
	case 14:
		*value |= static_cast<u64>(ptr[i++]) << 24;
		*value |= static_cast<u64>(ptr[i++]) << 16;
	case 13:
		*value |= static_cast<u64>(ptr[i++]) << 8;
	case 12:
		*value |= static_cast<u64>(ptr[i++]);
		break;

	default:
		*value = extra;
		break;
	}

	return i;
}


} // namespace Tech
