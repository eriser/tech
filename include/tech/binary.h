#ifndef TECH_BINARY_H
#define TECH_BINARY_H

#include <memory>
#include <vector>
#include <tech/types.h>


namespace Tech {


class Binary;
class ByteRef;

using BinaryList = std::vector<Binary>;


class Binary final {
public:
	enum SplitBahavior {
		kKeepEmptyParts,
		kSkipEmptyParts
	};

	static const size_t kNoPos = size_t(-1);

	Binary();
	Binary(const char* string, size_t size = kNoPos);
	Binary(const Binary& other);
	Binary(char ch);
	Binary(size_t size, char value);

	~Binary();

	Binary& operator=(const Binary& other);
	Binary& operator=(const char* string);
	Binary& operator=(char ch);

	bool operator==(const Binary& other) const;
	bool operator==(const char* string) const;

	bool operator!=(const Binary& other) const;
	bool operator!=(const char* string) const;

	bool operator<(const Binary& other) const;
	bool operator<(const char* string) const;

	bool operator<=(const Binary& other) const;
	bool operator<=(const char* string) const;

	bool operator>(const Binary& other) const;
	bool operator>(const char* string) const;

	bool operator>=(const Binary& other) const;
	bool operator>=(const char* string) const;

	Binary& operator+=(const Binary& other);
	Binary& operator+=(const char* string);
	Binary& operator+=(char ch);

	char operator[](size_t position) const;
	ByteRef operator[](size_t position);

	char at(size_t position) const;
	u8 byteAt(size_t position) const;

	bool isNull() const;
	bool isEmpty() const;
	size_t length() const;

	void clear();
	void swap(Binary& other);

	Binary left(size_t count) const;
	Binary middle(size_t from, size_t count = kNoPos) const;
	Binary right(size_t count) const;

	Binary leftRef(size_t count) const;
	Binary middleRef(size_t from, size_t count = kNoPos) const;
	Binary rightRef(size_t count) const;

	Binary leftJustified(size_t width, char fill = ' ', bool truncate = false) const;
	Binary middleJustified(size_t width, char fill = ' ', bool truncate = false) const;
	Binary rightJustified(size_t width, char fill = ' ', bool truncate = false) const;

	Binary& fill(char ch);

	bool startsWith(const char* string, size_t size = kNoPos) const;
	bool startsWith(const Binary& other) const;
	bool startsWith(char ch) const;

	bool endsWith(const char* string, size_t size = kNoPos) const;
	bool endsWith(const Binary& other) const;
	bool endsWith(char ch) const;

	BinaryList split(char sep, SplitBahavior behavior = kKeepEmptyParts) const;

	BinaryList split(const Binary& sep,
			SplitBahavior behavior = kKeepEmptyParts) const;

	void chop(size_t n);
	void truncate(size_t position);

	Binary simplified() const;
	Binary trimmed() const;

	Binary toHex(bool upperCase = false) const;

	static Binary fromHex(const char* string, size_t size = kNoPos);
	static Binary fromHex(const Binary& array);

	static Binary fromRawData(const char* data, size_t size = kNoPos);

	size_t spaceAtBegin() const;
	size_t spaceAtEnd() const;

	char* data();
	const char* data() const;
	const char* constData() const;

	Binary& append(const char* string, size_t size = kNoPos);
	Binary& append(const Binary &other);
	Binary& append(char ch);

	Binary& prepend(const char* string, size_t size = kNoPos);
	Binary& prepend(const Binary &other);
	Binary& prepend(char ch);

	Binary& insert(size_t position, const char* string, size_t size = kNoPos);

	Binary& insert(size_t position, const Binary& other);
	Binary& insert(size_t position, char ch);

	Binary& remove(size_t position, size_t count);
	Binary& remove(const char* string, size_t size = kNoPos);
	Binary& remove(const Binary& other);
	Binary& remove(char ch);

	Binary& replace(char before, const char* after, size_t afterSize = kNoPos);

	Binary& replace(char before, const Binary& after);
	Binary& replace(char before, char after);

	Binary& replace(const char* before, const char* after, size_t beforeSize = kNoPos,
			size_t afterSize = kNoPos);

	Binary& replace(const Binary& before, const Binary& after);
	Binary& replace(const char* before, char after, size_t beforeSize = kNoPos);

	Binary& replace(size_t position, size_t count, const char* after,
			size_t afterSize = kNoPos);

	Binary& replace(size_t position, size_t count, const Binary& after);
	Binary& replace(size_t position, size_t count, char after);

	size_t indexOf(const char* string, size_t size = kNoPos, size_t from = 0) const;

	size_t indexOf(const Binary& other, size_t from = 0) const;
	size_t indexOf(char ch, size_t from = 0) const;

	size_t lastIndexOf(const char* string, size_t size = kNoPos,
			size_t from = kNoPos) const;

	size_t lastIndexOf(const Binary& other, size_t from = kNoPos) const;
	size_t lastIndexOf(char ch, size_t from = kNoPos) const;

	Binary toUpper() const;
	Binary toLower() const;

	int resize(size_t newLength);
	int resize(size_t newLength, char value);

	operator const char*() const;
	operator const void*() const;

private:
	struct Buffer;
	friend class ByteRef;

	Buffer* buffer_;
	char* begin_;
	char* end_;

	static std::unique_ptr<Buffer> sharedNull_;
	static Buffer* sharedNull();
	static Binary uninitialized(size_t size);


	Binary(Buffer* buffer);
	Binary(const Binary& other, size_t position, size_t count);

	size_t hashingSearch(const char* string, size_t size, size_t from) const;
	size_t hashingReverseSearch(const char* string, size_t size, size_t from) const;

	size_t boyerMooreSearch(const char* string, size_t size, size_t from) const;
	size_t boyerMooreReverseSearch(const char* string, size_t size, size_t from) const;

	void makeUnique();
};


bool operator==(const char* string, const Binary& array);
bool operator!=(const char* string, const Binary& array);
bool operator<(const char* string, const Binary& array);
bool operator>(const char* string, const Binary& array);
bool operator<=(const char* string, const Binary& array);
bool operator>=(const char* string, const Binary& array);
Binary operator+(const Binary& a1, const Binary& a2);
Binary operator+(const char* string, const Binary& array);
Binary operator+(const Binary& array, const char* string);
Binary operator+(char ch, const Binary& array);
Binary operator+(const Binary& array, char ch);


class ByteRef {
public:
	operator char() const
	{
		if(pos_ < array_.length())
			return array_.constData()[pos_];

		return char(0);
	}


	ByteRef& operator=(const char ch)
	{
		array_.data()[pos_] = ch;
		return *this;
	}


	ByteRef& operator=(const ByteRef& ref)
	{
		array_.data()[pos_] = ref.array_.constData()[ref.pos_];
		return *this;
	}


	// ch имеет тип int чтобы избежать неоднозначности со встроенным оператором, если в
	// качестве аргумента используется значение > 127.
	bool operator==(int ch) const
	{
		return array_.constData()[pos_] == (ch & 0xFF);
	}


	bool operator!=(int ch) const
	{
		return array_.constData()[pos_] != (ch & 0xFF);
	}


	bool operator<(int ch) const
	{
		return array_.constData()[pos_] < (ch & 0xFF);
	}


	bool operator>(int ch) const
	{
		return array_.constData()[pos_] > (ch & 0xFF);
	}


	bool operator<=(int ch) const
	{
		return array_.constData()[pos_] <= (ch & 0xFF);
	}


	bool operator>=(int ch) const
	{
		return array_.constData()[pos_] >= (ch & 0xFF);
	}


private:
	friend class Binary;

	Binary& array_;
	size_t pos_;


	ByteRef(Binary& array, size_t pos) :
		array_(array),
		pos_(pos)
	{
	}
};


inline bool operator==(int ch, const ByteRef& ref)
{
	return ref == ch;
}


inline bool operator!=(int ch, const ByteRef& ref)
{
	return ref != ch;
}


inline bool operator<(int ch, const ByteRef& ref)
{
	return ref > ch;
}


inline bool operator>(int ch, const ByteRef& ref)
{
	return ref < ch;
}


inline bool operator<=(int ch, const ByteRef& ref)
{
	return ref >= ch;
}


inline bool operator>=(int ch, const ByteRef& ref)
{
	return ref <= ch;
}


} // namespace Tech


#endif // TECH_BINARY_H
