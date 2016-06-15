#ifndef TECH_ITEM_H
#define TECH_ITEM_H

#include <vector>
#include <tech/binary.h>
#include <tech/string.h>
#include <tech/types.h>


namespace Tech {


enum class ItemType : u8 {
	kNull,
	kBool,
	kInteger,
	kUnsigned,
	kReal,
	kBinary,
	kString,
	kArray,
	kMap
};


class Item {
public:
	using Field = Pair<Tech::String, Item>;

	Item();
	Item(bool value);
	Item(i8 value);
	Item(i16 value);
	Item(i32 value);
	Item(i64 value);
	Item(u8 value);
	Item(u16 value);
	Item(u32 value);
	Item(u64 value);
	Item(double value);
	Item(const u8* buffer, size_t length);
	Item(const Tech::Binary& binary);
	Item(const char* string, size_t length = -1);
	Item(const Tech::String& string);
	Item(const Item& item);
	Item(Item&& item);

	~Item();

	Item& operator=(const Item& other);

	bool operator<(const Item& other) const;
	bool operator<=(const Item& other) const;
	bool operator>=(const Item& other) const;
	bool operator>(const Item& other) const;
	bool operator==(const Item& other) const;
	bool operator!=(const Item& other) const;

	bool operator!() const;

	ItemType type() const;

	bool isNull() const;
	bool isBool() const;
	bool isInteger() const;
	bool isUnsigned() const;
	bool isIntegral() const;
	bool isReal() const;
	bool isNumeric() const;
	bool isBinary() const;
	bool isString() const;
	bool isArray() const;
	bool isMap() const;

	bool toBool() const;
	i64 toInteger() const;
	u64 toUnsigned() const;
	double toReal() const;
	Tech::Binary toBinary() const;
	Tech::String toString() const;

	bool hasTag() const ;
	u32 tag() const;
	void setTag(u32 tag);

	uint count() const;

	Item& operator[](uint index);
	Item& operator[](uint index) const;
	Item& operator[](const Tech::Binary& name);
	Item& operator[](const Tech::Binary& name) const;

	bool hasField(const Tech::Binary& name) const;
	Field& field(uint index) const;

	Item get(uint index, const Item& defaultValue = Item()) const;
	Item get(const Tech::Binary& name, const Item& defaultValue = Item()) const;

	void swap(Item& other);

private:
	union {
		bool bool_;
		i64 integer_;
		u64 unsigned_;
		double real_;
		Tech::Binary* binary_;
		Tech::String* string_;
		std::vector<Item>* array_;
		std::vector<Field>* map_;
		void* rawValue_;
	};

	ItemType type_;
	u32 tag_;

	void deletePayload();
	static bool fieldLessThan(const Field& field, const Tech::Binary& name);
};


} // namespace Tech


#endif // TECH_ITEM_H
