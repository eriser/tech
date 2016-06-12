#include "../include/tech/item.h"

#include <cassert>
#include <tech/format.h>


namespace Tech {


Item::Item() :
	rawValue_(nullptr),
	type_(ItemType::kNull),
	tag_(0)
{
}


Item::Item(bool value) :
	bool_(value),
	type_(ItemType::kBool),
	tag_(0)
{
}


Item::Item(i8 value) :
	integer_(value),
	type_(ItemType::kInteger),
	tag_(0)
{
}


Item::Item(i16 value) :
	integer_(value),
	type_(ItemType::kInteger),
	tag_(0)
{
}


Item::Item(i32 value) :
	integer_(value),
	type_(ItemType::kInteger),
	tag_(0)
{
}


Item::Item(i64 value) :
	integer_(value),
	type_(ItemType::kInteger),
	tag_(0)
{
}


Item::Item(u8 value) :
	unsigned_(value),
	type_(ItemType::kUnsigned),
	tag_(0)
{
}


Item::Item(u16 value) :
	unsigned_(value),
	type_(ItemType::kUnsigned),
	tag_(0)
{
}


Item::Item(u32 value) :
	unsigned_(value),
	type_(ItemType::kUnsigned),
	tag_(0)
{
}


Item::Item(u64 value) :
	unsigned_(value),
	type_(ItemType::kUnsigned),
	tag_(0)
{
}


Item::Item(double value) :
	real_(value),
	type_(ItemType::kReal),
	tag_(0)
{
}


Item::Item(const u8* buffer, size_t length) :
	binary_(new Tech::ByteArray(reinterpret_cast<const char*>(buffer), length)),
	type_(ItemType::kBinary),
	tag_(0)
{
}


Item::Item(const Tech::ByteArray& binary) :
	binary_(new Tech::ByteArray(binary)),
	type_(ItemType::kBinary),
	tag_(0)
{
}


Item::Item(const char* string, size_t length) :
	string_(new Tech::String(string, length)),
	type_(ItemType::kString),
	tag_(0)
{
}


Item::Item(const Tech::String& string) :
	string_(new Tech::String(string)),
	type_(ItemType::kString),
	tag_(0)
{
}


Item::Item(const Item& item)
{
	*this = item;
}


Item::Item(Item&& item) :
	type_(ItemType::kNull)
{
	swap(item);
}


Item::~Item()
{
	deletePayload();
}


Item& Item::operator=(const Item& other)
{
	deletePayload();

	if(other.isBinary()) {
		binary_ = new Tech::ByteArray(*other.binary_);
		*binary_ = *other.binary_;
	}
	else if(other.isString()) {
		string_ = new Tech::String(*other.string_);
		*string_ = *other.string_;
	}
	else if(other.isArray()) {
		array_ = new std::vector<Item>(*other.array_);
		*array_ = *other.array_;
	}
	else if(other.isMap()) {
		map_ = new std::vector<Field>(*other.map_);
	}
	else {
		rawValue_ = other.rawValue_;
	}

	type_ = other.type_;
	tag_ = other.tag_;
	return *this;
}


bool Item::operator<(const Item& other) const
{
	if(type_ < other.type_ || tag_ < other.tag_) {
		return true;
	}
	else if(type_ > other.type_ || tag_ > other.tag_) {
		return false;
	}

	switch(type_) {
	case ItemType::kNull:
		return false;

	case ItemType::kBool:
		return bool_ < other.bool_;

	case ItemType::kInteger:
		return integer_ < other.integer_;

	case ItemType::kUnsigned:
		return unsigned_ < other.unsigned_;

	case ItemType::kReal:
		return real_ < other.real_;

	case ItemType::kBinary:
		return *binary_ < *other.binary_;

	case ItemType::kString:
		return *string_ < *other.string_;

	case ItemType::kArray:
		return *array_ < *other.array_;

	case ItemType::kMap:
		return *map_ < *other.map_;
	}

	return false;
}


bool Item::operator<=(const Item& other) const
{
	if(type_ < other.type_ || tag_ < other.tag_) {
		return true;
	}
	else if(type_ > other.type_ || tag_ > other.tag_) {
		return false;
	}

	switch(type_) {
	case ItemType::kNull:
		return true;

	case ItemType::kBool:
		return bool_ <= other.bool_;

	case ItemType::kInteger:
		return integer_ <= other.integer_;

	case ItemType::kUnsigned:
		return unsigned_ <= other.unsigned_;

	case ItemType::kReal:
		return real_ <= other.real_;

	case ItemType::kBinary:
		return *binary_ <= *other.binary_;

	case ItemType::kString:
		return *string_ <= *other.string_;

	case ItemType::kArray:
		return *array_ <= *other.array_;

	case ItemType::kMap:
		return *map_ <= *other.map_;
	}

	return false;
}


bool Item::operator>=(const Item& other) const
{
	if(type_ < other.type_ || tag_ < other.tag_) {
		return false;
	}
	else if(type_ > other.type_ || tag_ > other.tag_) {
		return true;
	}

	switch(type_) {
	case ItemType::kNull:
		return true;

	case ItemType::kBool:
		return bool_ >= other.bool_;

	case ItemType::kInteger:
		return integer_ >= other.integer_;

	case ItemType::kUnsigned:
		return unsigned_ >= other.unsigned_;

	case ItemType::kReal:
		return real_ >= other.real_;

	case ItemType::kBinary:
		return *binary_ >= *other.binary_;

	case ItemType::kString:
		return *string_ >= *other.string_;

	case ItemType::kArray:
		return *array_ >= *other.array_;

	case ItemType::kMap:
		return *map_ >= *other.map_;
	}

	return false;
}


bool Item::operator>(const Item& other) const
{
	if(type_ < other.type_ || tag_ < other.tag_) {
		return false;
	}
	else if(type_ > other.type_ || tag_ > other.tag_) {
		return true;
	}

	switch(type_) {
	case ItemType::kNull:
		return false;

	case ItemType::kBool:
		return bool_ > other.bool_;

	case ItemType::kInteger:
		return integer_ > other.integer_;

	case ItemType::kUnsigned:
		return unsigned_ > other.unsigned_;

	case ItemType::kReal:
		return real_ > other.real_;

	case ItemType::kBinary:
		return *binary_ > *other.binary_;

	case ItemType::kString:
		return *string_ > *other.string_;

	case ItemType::kArray:
		return *array_ > *other.array_;

	case ItemType::kMap:
		return *map_ > *other.map_;
	}

	return false;
}


bool Item::operator==(const Item& other) const
{
	if(type_ != other.type_ || tag_ != other.tag_)
		return false;

	switch(type_) {
	case ItemType::kNull:
		return true;

	case ItemType::kBool:
		return bool_ == other.bool_;

	case ItemType::kInteger:
		return integer_ == other.integer_;

	case ItemType::kUnsigned:
		return unsigned_ == other.unsigned_;

	case ItemType::kReal:
		return real_ == other.real_;

	case ItemType::kBinary:
		return *binary_ == *other.binary_;

	case ItemType::kString:
		return *string_ == *other.string_;

	case ItemType::kArray:
		return *array_ == *other.array_;

	case ItemType::kMap:
		return *map_ == *other.map_;
	}

	return false;
}


bool Item::operator!=(const Item& other) const
{
	return *this != other;
}


bool Item::operator!() const
{
	return !isNull();
}


ItemType Item::type() const
{
	return type_;
}


bool Item::isNull() const
{
	return type_ == ItemType::kNull;
}


bool Item::isBool() const
{
	return type_ == ItemType::kBool;
}


bool Item::isInteger() const
{
	return type_ == ItemType::kInteger;
}


bool Item::isUnsigned() const
{
	return type_ == ItemType::kUnsigned;
}


bool Item::isIntegral() const
{
	return type_ == ItemType::kInteger || type_ == ItemType::kUnsigned;
}


bool Item::isReal() const
{
	return type_ == ItemType::kReal;
}


bool Item::isNumeric() const
{
	return isIntegral() || type_ == ItemType::kReal;
}


bool Item::isBinary() const
{
	return type_ == ItemType::kBinary;
}


bool Item::isString() const
{
	return type_ == ItemType::kString;
}


bool Item::isArray() const
{
	return type_ == ItemType::kArray;
}


bool Item::isMap() const
{
	return type_ == ItemType::kMap;
}


bool Item::toBool() const
{
	switch(type_) {
	case ItemType::kNull:
		return false;

	case ItemType::kBool:
		return bool_;

	case ItemType::kInteger:
		return integer_ != 0;

	case ItemType::kUnsigned:
		return unsigned_ != 0;

	case ItemType::kReal:
		return real_ != 0.0;

	default:
		return false;
	}
}


i64 Item::toInteger() const
{
	switch(type_) {
	case ItemType::kNull:
		return 0;

	case ItemType::kBool:
		return bool_;

	case ItemType::kInteger:
		return integer_;

	case ItemType::kUnsigned:
		return unsigned_;

	case ItemType::kReal:
		return static_cast<i64>(real_);

	default:
		return 0;
	}
}


u64 Item::toUnsigned() const
{
	switch(type_) {
	case ItemType::kNull:
		return 0;

	case ItemType::kBool:
		return static_cast<u64>(bool_);

	case ItemType::kInteger:
		return static_cast<u64>(integer_);

	case ItemType::kUnsigned:
		return unsigned_;

	case ItemType::kReal:
		return static_cast<u64>(real_);

	default:
		return 0;
	}
}


double Item::toReal() const
{
	switch(type_) {
	case ItemType::kNull:
		return 0;

	case ItemType::kBool:
		return bool_;

	case ItemType::kInteger:
		return integer_;

	case ItemType::kUnsigned:
		return unsigned_;

	case ItemType::kReal:
		return real_;

	default:
		return 0.0;
	}
}


Tech::ByteArray Item::toBinary() const
{
	if(!isBinary())
		return Tech::ByteArray();

	return *binary_;
}


Tech::String Item::toString() const
{
	switch(type_) {
	case ItemType::kBool:
		return Tech::formatValue(bool_);

	case ItemType::kInteger:
		return Tech::formatValue(integer_);

	case ItemType::kUnsigned:
		return Tech::formatValue(unsigned_);

	case ItemType::kReal:
		return Tech::formatValue(real_);

	case ItemType::kBinary:
		return binary_->toHex();

	case ItemType::kString:
		return *string_;

	default:
		return Tech::String();
	}
}


bool Item::hasTag() const
{
	return tag_ > 0;
}


u32 Item::tag() const
{
	return tag_;
}


void Item::setTag(u32 tag)
{
	tag_ = tag;
}


uint Item::count() const
{
	if(isBinary()) {
		return binary_->length();
	}
	else if(isString()) {
		return string_->length();
	}
	else if(isArray()) {
		return static_cast<uint>(array_->size());
	}
	else if(isMap()) {
		return static_cast<uint>(map_->size());
	}

	return 0;
}


bool Item::hasField(const Tech::ByteArray& name) const
{
	if(isMap()) {
		auto it = std::lower_bound(map_->begin(), map_->end(), name, fieldLessThan);
		return it != map_->end() && it->first == name;
	}

	return false;
}


Item& Item::operator[](uint index)
{
	if(!isArray())
		deletePayload();

	if(isNull()) {
		type_ = ItemType::kArray;
		array_ = new std::vector<Item>;
	}

	if(index >= array_->size()) {
		array_->emplace_back(Item());
		return array_->back();
	}

	return (*array_)[index];
}


Item& Item::operator[](uint index) const
{
	assert(isArray() && index < array_->size());
	return (*array_)[index];
}


Item& Item::operator[](const Tech::ByteArray& name)
{
	if(!isMap())
		deletePayload();

	if(isNull()) {
		type_ = ItemType::kMap;
		map_ = new std::vector<Field>;
	}

	auto it = std::lower_bound(map_->begin(), map_->end(), name, fieldLessThan);
	if(it == map_->end() || it->first != name)
		it = map_->emplace(it, makePair(name, Item()));

	return it->second;
}


Item& Item::operator[](const Tech::ByteArray& name) const
{
	assert(isMap());
	auto it = std::lower_bound(map_->begin(), map_->end(), name, fieldLessThan);
	assert(it != map_->end() && it->first == name);
	return it->second;
}


Item::Field& Item::field(uint index) const
{
	assert(isMap() && index < map_->size());
	return *(map_->data() + index);
}


Item Item::get(uint index, const Item& defaultValue) const
{
	if(!isArray() || index >= array_->size())
		return defaultValue;

	return (*array_)[index];
}


Item Item::get(const Tech::ByteArray& name, const Item& defaultValue) const
{
	if(isMap()) {
		auto it = std::lower_bound(map_->begin(), map_->end(), name, fieldLessThan);
		if(it != map_->end() && it->first == name)
			return it->second;
	}

	return defaultValue;
}


void Item::swap(Item& other)
{
	std::swap(rawValue_, other.rawValue_);
	std::swap(type_, other.type_);
	std::swap(tag_, other.tag_);
}


void Item::deletePayload()
{
	if(isNull())
		return;

	if(isBinary()) {
		delete binary_;
	}
	else if(isString()) {
		delete string_;
	}
	else if(isArray()) {
		delete array_;
	}
	else if(isMap()) {
		delete map_;
	}

	type_ = ItemType::kNull;
}


bool Item::fieldLessThan(const Field& field, const Tech::ByteArray& name)
{
	return field.first < name;
}


} // namespace Tech
