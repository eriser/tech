#ifndef TECH_BYTEARRAY_H
#define TECH_BYTEARRAY_H

#include <memory>
#include <vector>
#include <tech/types.h>


namespace Tech {


class ByteArray;
class ByteRef;

using ByteArrayList = std::vector<ByteArray>;


class ByteArray final {
public:
	enum SplitBahavior {
		kKeepEmptyParts,
		kSkipEmptyParts
	};

	static const size_t kNoPos = size_t(-1);

	ByteArray();
	ByteArray(const char* string, size_t size = kNoPos);
	ByteArray(const ByteArray& other);
	ByteArray(char ch);
	ByteArray(size_t size, char value);

	~ByteArray();

	ByteArray& operator=(const ByteArray& other);
	ByteArray& operator=(const char* string);
	ByteArray& operator=(char ch);

	bool operator==(const ByteArray& other) const;
	bool operator==(const char* string) const;

	bool operator!=(const ByteArray& other) const;
	bool operator!=(const char* string) const;

	bool operator<(const ByteArray& other) const;
	bool operator<(const char* string) const;

	bool operator<=(const ByteArray& other) const;
	bool operator<=(const char* string) const;

	bool operator>(const ByteArray& other) const;
	bool operator>(const char* string) const;

	bool operator>=(const ByteArray& other) const;
	bool operator>=(const char* string) const;

	ByteArray& operator+=(const ByteArray& other);
	ByteArray& operator+=(const char* string);
	ByteArray& operator+=(char ch);

	char operator[](size_t position) const;
	ByteRef operator[](size_t position);

	char at(size_t position) const;
	u8 byteAt(size_t position) const;

	bool isNull() const;
	bool isEmpty() const;
	size_t length() const;

	void clear();
	void swap(ByteArray& other);

	ByteArray left(size_t count) const;
	ByteArray middle(size_t from, size_t count = kNoPos) const;
	ByteArray right(size_t count) const;

	ByteArray leftRef(size_t count) const;
	ByteArray middleRef(size_t from, size_t count = kNoPos) const;
	ByteArray rightRef(size_t count) const;

	ByteArray leftJustified(size_t width, char fill = ' ', bool truncate = false) const;
	ByteArray middleJustified(size_t width, char fill = ' ', bool truncate = false) const;
	ByteArray rightJustified(size_t width, char fill = ' ', bool truncate = false) const;

	ByteArray& fill(char ch);

	bool startsWith(const char* string, size_t size = kNoPos) const;
	bool startsWith(const ByteArray& other) const;
	bool startsWith(char ch) const;

	bool endsWith(const char* string, size_t size = kNoPos) const;
	bool endsWith(const ByteArray& other) const;
	bool endsWith(char ch) const;

	ByteArrayList split(char sep, SplitBahavior behavior = kKeepEmptyParts) const;

	ByteArrayList split(const ByteArray& sep,
			SplitBahavior behavior = kKeepEmptyParts) const;

	void chop(size_t n);
	void truncate(size_t position);

	ByteArray simplified() const;
	ByteArray trimmed() const;

	ByteArray toHex(bool upperCase = false) const;

	static ByteArray fromHex(const char* string, size_t size = kNoPos);
	static ByteArray fromHex(const ByteArray& array);

	static ByteArray fromRawData(const char* data, size_t size = kNoPos);

	size_t spaceAtBegin() const;
	size_t spaceAtEnd() const;

	char* data();
	const char* data() const;
	const char* constData() const;

	ByteArray& append(const char* string, size_t size = kNoPos);
	ByteArray& append(const ByteArray &other);
	ByteArray& append(char ch);

	ByteArray& prepend(const char* string, size_t size = kNoPos);
	ByteArray& prepend(const ByteArray &other);
	ByteArray& prepend(char ch);

	ByteArray& insert(size_t position, const char* string, size_t size = kNoPos);

	ByteArray& insert(size_t position, const ByteArray& other);
	ByteArray& insert(size_t position, char ch);

	ByteArray& remove(size_t position, size_t count);
	ByteArray& remove(const char* string, size_t size = kNoPos);
	ByteArray& remove(const ByteArray& other);
	ByteArray& remove(char ch);

	ByteArray& replace(char before, const char* after, size_t afterSize = kNoPos);

	ByteArray& replace(char before, const ByteArray& after);
	ByteArray& replace(char before, char after);

	ByteArray& replace(const char* before, const char* after, size_t beforeSize = kNoPos,
			size_t afterSize = kNoPos);

	ByteArray& replace(const ByteArray& before, const ByteArray& after);
	ByteArray& replace(const char* before, char after, size_t beforeSize = kNoPos);

	ByteArray& replace(size_t position, size_t count, const char* after,
			size_t afterSize = kNoPos);

	ByteArray& replace(size_t position, size_t count, const ByteArray& after);
	ByteArray& replace(size_t position, size_t count, char after);

	size_t indexOf(const char* string, size_t size = kNoPos, size_t from = 0) const;

	size_t indexOf(const ByteArray& other, size_t from = 0) const;
	size_t indexOf(char ch, size_t from = 0) const;

	size_t lastIndexOf(const char* string, size_t size = kNoPos,
			size_t from = kNoPos) const;

	size_t lastIndexOf(const ByteArray& other, size_t from = kNoPos) const;
	size_t lastIndexOf(char ch, size_t from = kNoPos) const;

	ByteArray toUpper() const;
	ByteArray toLower() const;

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
	static ByteArray uninitialized(size_t size);


	ByteArray(Buffer* buffer);
	ByteArray(const ByteArray& other, size_t position, size_t count);

	size_t hashingSearch(const char* string, size_t size, size_t from) const;
	size_t hashingReverseSearch(const char* string, size_t size, size_t from) const;

	size_t boyerMooreSearch(const char* string, size_t size, size_t from) const;
	size_t boyerMooreReverseSearch(const char* string, size_t size, size_t from) const;

	void makeUnique();
};


bool operator==(const char* string, const ByteArray& array);
bool operator!=(const char* string, const ByteArray& array);
bool operator<(const char* string, const ByteArray& array);
bool operator>(const char* string, const ByteArray& array);
bool operator<=(const char* string, const ByteArray& array);
bool operator>=(const char* string, const ByteArray& array);
ByteArray operator+(const ByteArray& a1, const ByteArray& a2);
ByteArray operator+(const char* string, const ByteArray& array);
ByteArray operator+(const ByteArray& array, const char* string);
ByteArray operator+(char ch, const ByteArray& array);
ByteArray operator+(const ByteArray& array, char ch);


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
	friend class ByteArray;

	ByteArray& array_;
	size_t pos_;


	ByteRef(ByteArray& array, size_t pos) :
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


#endif // TECH_BYTEARRAY_H
