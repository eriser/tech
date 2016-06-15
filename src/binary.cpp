#include <tech/binary.h>

#include <algorithm>
#include <atomic>
#include <cstring>
#include <tech/utils.h>


// remove definitions from atomic -> atomic_base.h -> stdbool.h
#undef bool
#undef true
#undef false


namespace Tech {


struct Binary::Buffer {
	std::atomic<uint> rc;

	// capacity содержит значение размера выделенного буфера, а в случае, если
	// используется внешний буфер (Binary сконструирован через вызов fromRawData()),
	// значение capacity устанавливается в 0. Поэтому во всех функциях, которые
	// используют capacity, требуется учитывать этот случай.
	size_t capacity;
	char* data;

	static const size_t kMinCapacity = 16;


	// Используется только при создании shared null экземпляра. Для защиты этого объекта
	// от изменения и удаления, счетчик ссылок устанавливается в единицу, несмотря на то,
	// что ссылок на объект еще не существует.
	Buffer() :
		rc(1),
		capacity(0),
		data(const_cast<char*>(""))
	{
	}


	// Предполагается, что вызывающий код становится владельцем нового буфера, поэтому
	// счетчик ссылок изначально устанавливается в единицу, чтобы не выполнять лишнюю
	// атомарную операцию при acquire().
	Buffer(size_t cap) :
		rc(1),
		capacity(cap),
		data(reinterpret_cast<char*>(this + 1))
	{
	}


	void acquire()
	{
		rc++;
	}


	// Т.к. Buffer состоит из простых типов данных, вызывать деструктор здесь не
	// обязательно, достаточно простого освобождения памяти. Статический shared null
	// объект инициализируется со счетчиком ссылок, равным 1, поэтому попытки удалить его
	// здесь никогда не будет.
	void release()
	{
		if(--rc == 0)
			operator delete(this);
	}


	bool isUnique() const
	{
		return rc < 2 && capacity;
	}


	// beginFor() не должна вызываться для экземпляров, созданных с помощью вызова
	// fromRawData()
	char* beginFor(size_t length) const
	{
		return data + (capacity - length) / 2;
	}


	static Buffer* allocate(size_t length)
	{
		size_t capacity = capacityFor(length);
		void* result = operator new(sizeof(Buffer) + capacity);

		// Вычитая единицу мы резервируем место для завершающего 0
		return new (result) Buffer(capacity - 1);
	}


	static size_t capacityFor(size_t length)
	{
		return std::max(kMinCapacity, ceilToPowerOfTwo(length * 2));
	}
};


const size_t Binary::Buffer::kMinCapacity;
std::unique_ptr<Binary::Buffer> Binary::sharedNull_;


Binary::Binary() :
	buffer_(sharedNull()),
	begin_(nullptr),
	end_(nullptr)
{
	buffer_->acquire();
}


Binary::Binary(const char* string, size_t size)
{
	if(size == kNoPos)
		size = std::char_traits<char>::length(string);

	buffer_ = Buffer::allocate(size);
	begin_ = buffer_->beginFor(size);
	end_ = std::copy(string, string + size, begin_);
	*end_ = '\0';
}


Binary::Binary(const Binary& other)
{
	other.buffer_->acquire();
	buffer_ = other.buffer_;
	begin_ = other.begin_;
	end_ = other.end_;
}


Binary::Binary(char ch)
{
	buffer_ = Buffer::allocate(1);
	begin_ = buffer_->beginFor(1);
	*begin_ = ch;
	end_ = begin_ + 1;
	*end_ = '\0';
}


Binary::Binary(size_t size, char value)
{
	buffer_ = Buffer::allocate(size);
	begin_ = buffer_->beginFor(size);
	end_ = begin_ + size;
	*end_ = '\0';

	std::fill(begin_, end_, value);
}


Binary::~Binary()
{
	buffer_->release();
}


Binary& Binary::operator=(const Binary& other)
{
	if(buffer_ != other.buffer_) {
		other.buffer_->acquire();

		Buffer* old = buffer_;
		buffer_ = other.buffer_;
		begin_ = other.begin_;
		end_ = other.end_;
		old->release();
	}
	else {
		begin_ = other.begin_;
		end_ = other.end_;
	}

	return *this;
}


Binary& Binary::operator=(const char* string)
{
	size_t size = std::char_traits<char>::length(string);

	if(!buffer_->isUnique() || buffer_->capacity < size) {
		buffer_->release();
		buffer_ = Buffer::allocate(size);
	}

	begin_ = buffer_->beginFor(size);
	end_ = std::copy(string, string + size, begin_);
	*end_ = '\0';
	return *this;
}


Binary& Binary::operator=(char ch)
{
	if(!buffer_->isUnique()) {
		buffer_->release();
		buffer_ = Buffer::allocate(1);
	}

	begin_ = buffer_->beginFor(1);
	*begin_ = ch;

	end_ = begin_ + 1;
	*end_ = '\0';
	return *this;
}


bool Binary::operator==(const Binary& other) const
{
	if(buffer_ == other.buffer_)
		return true;

	if(length() != other.length())
		return false;

	return std::equal(begin_, end_, other.begin_);
}


bool Binary::operator==(const char* string) const
{
	size_t size = std::char_traits<char>::length(string);
	if(size != length())
		return false;

	return std::equal(begin_, end_, string);
}


bool Binary::operator!=(const Binary& other) const
{
	return !(*this == other);
}


bool Binary::operator!=(const char* string) const
{
	return !(*this == string);
}


bool Binary::operator<(const Binary& other) const
{
	return std::lexicographical_compare(begin_, end_, other.begin_, other.end_);
}


bool Binary::operator<(const char* string) const
{
	size_t size = std::char_traits<char>::length(string);
	return std::lexicographical_compare(begin_, end_, string, string + size);
}


bool Binary::operator<=(const Binary& other) const
{
	auto func = [](char c1, char c2) -> bool { return c1 <= c2; };
	return std::lexicographical_compare(begin_, end_, other.begin_, other.end_, func);
}


bool Binary::operator<=(const char* string) const
{
	size_t size = std::char_traits<char>::length(string);
	auto func = [](char c1, char c2) -> bool { return c1 <= c2; };
	return std::lexicographical_compare(begin_, end_, string, string + size, func);
}


bool Binary::operator>(const Binary& other) const
{
	auto func = [](char c1, char c2) -> bool { return c1 > c2; };
	return std::lexicographical_compare(begin_, end_, other.begin_, other.end_, func);
}


bool Binary::operator>(const char* string) const
{
	size_t size = std::char_traits<char>::length(string);
	auto func = [](char c1, char c2) -> bool { return c1 > c2; };
	return std::lexicographical_compare(begin_, end_, string, string + size, func);
}


bool Binary::operator>=(const Binary& other) const
{
	auto func = [](char c1, char c2) -> bool { return c1 >= c2; };
	return std::lexicographical_compare(begin_, end_, other.begin_, other.end_, func);
}


bool Binary::operator>=(const char* string) const
{
	size_t size = std::char_traits<char>::length(string);
	auto func = [](char c1, char c2) -> bool { return c1 >= c2; };
	return std::lexicographical_compare(begin_, end_, string, string + size, func);
}


Binary& Binary::operator+=(const Binary& other)
{
	return append(other);
}


Binary& Binary::operator+=(const char* string)
{
	return append(string);

}


Binary& Binary::operator+=(char ch)
{
	return append(ch);
}


char Binary::operator[](size_t position) const
{
	return begin_[position];
}


ByteRef Binary::operator[](size_t position)
{
	return ByteRef(*this, position);
}


char Binary::at(size_t position) const
{
	return begin_[position];
}


u8 Binary::byteAt(size_t position) const
{
	return *reinterpret_cast<u8*>(begin_ + position);
}


size_t Binary::spaceAtBegin() const
{
	return begin_ - buffer_->data;
}


size_t Binary::spaceAtEnd() const
{
	if(buffer_->capacity)
		return buffer_->capacity - (end_ - buffer_->data);

	return 0;
}


bool Binary::isNull() const
{
	return buffer_ == sharedNull();
}


bool Binary::isEmpty() const
{
	return begin_ == end_;
}


size_t Binary::length() const
{
	return end_ - begin_;
}


void Binary::clear()
{
	if(!buffer_->isUnique()) {
		buffer_->release();
		buffer_ = Buffer::allocate(0);
	}

	begin_ = buffer_->beginFor(0);
	end_ = begin_;
	*end_ = '\0';
}


void Binary::swap(Binary& other)
{
	std::swap(buffer_, other.buffer_);
	std::swap(begin_, other.begin_);
	std::swap(end_, other.end_);
}


Binary Binary::left(size_t count) const
{
	count = std::min(count, length());

	Binary result = Binary::uninitialized(count);
	std::copy(begin_, begin_ + count, result.begin_);

	*result.end_ = '\0';
	return result;
}


Binary Binary::middle(size_t from, size_t count) const
{
	from = std::min(from, length());
	count = std::min(count, length() - from);

	Binary result = Binary::uninitialized(count);
	std::copy(begin_ + from, begin_ + from + count, result.begin_);

	*result.end_ = '\0';
	return result;
}


Binary Binary::right(size_t count) const
{
	count = std::min(count, length());
	size_t from = length() - count;

	Binary result = Binary::uninitialized(count);
	std::copy(begin_ + from, begin_ + from + count, result.begin_);

	*result.end_ = '\0';
	return result;
}


Binary Binary::leftRef(size_t count) const
{
	count = std::min(count, length());
	return Binary(*this, 0, count);
}


Binary Binary::middleRef(size_t from, size_t count) const
{
	from = std::min(from, length());
	count = std::min(count, length() - from);
	return Binary(*this, from, count);
}


Binary Binary::rightRef(size_t count) const
{
	count = std::min(count, length());
	size_t from = length() - count;
	return Binary(*this, from, count);
}


Binary Binary::leftJustified(size_t width, char fill, bool truncate) const
{
	if(width >= length()) {
		Binary result = *this;
		size_t count = width - length();

		while(count--)
			result.append(fill);

		return result;
	}
	else if(truncate) {
		return left(width);
	}

	return *this;
}


Binary Binary::middleJustified(size_t width, char fill, bool truncate) const
{
	if(width >= length()) {
		Binary result = *this;
		size_t count = width - length();
		size_t halfCount = count / 2;

		while(count-- != halfCount)
			result.prepend(fill);

		count++;

		while(count--)
			result.append(fill);

		return result;
	}
	else if(truncate) {
		return middle((length() - width) / 2, width);
	}

	return *this;
}


Binary Binary::rightJustified(size_t width, char fill, bool truncate) const
{
	if(width >= length()) {
		Binary result = *this;
		size_t count = width - length();

		while(count--)
			result.prepend(fill);

		return result;
	}
	else if(truncate) {
		return right(width);
	}

	return *this;
}


Binary& Binary::fill(char ch)
{
	makeUnique();
	std::fill(begin_, end_, ch);
	return *this;
}


bool Binary::startsWith(const char* string, size_t size) const
{
	if(size == kNoPos)
		size = std::char_traits<char>::length(string);

	if(size > length())
		return false;

	return std::equal(string, string + size, begin_);
}


bool Binary::startsWith(const Binary& other) const
{
	return startsWith(other.begin_, other.length());
}


bool Binary::startsWith(char ch) const
{
	return startsWith(&ch, 1);
}


bool Binary::endsWith(const char* string, size_t size) const
{
	if(size == kNoPos)
		size = std::char_traits<char>::length(string);

	if(size > length())
		return false;

	return std::equal(string, string + size, end_ - size);
}


bool Binary::endsWith(const Binary& other) const
{
	return endsWith(other.begin_, other.length());
}


bool Binary::endsWith(char ch) const
{
	return endsWith(&ch, 1);
}


BinaryList Binary::split(char sep, SplitBahavior behavior) const
{
	BinaryList result;

	char* begin = begin_;
	char* end = begin_;

	while(end != end_) {
		size_t position = std::distance(begin_, begin);

		end = std::find(begin, end_, sep);

		size_t count = std::distance(begin, end);
		begin = end + 1;

		if(count > 0 || behavior == kKeepEmptyParts)
			result.emplace_back(middle(position, count));
	}

	return result;
}


BinaryList Binary::split(const Binary& sep, SplitBahavior behavior) const
{
	BinaryList result;

	size_t begin = 0;
	size_t end = 0;

	while(end != length()) {
		size_t position = begin;
		end = std::min(indexOf(sep, begin), length());

		size_t count = end - begin;
		begin = end + sep.length();

		if(count > 0 || behavior == kKeepEmptyParts)
			result.emplace_back(middle(position, count));
	}

	return result;
}


void Binary::chop(size_t n)
{
	if(n == 0)
		return;

	makeUnique();
	end_ -= std::min(n, length());
	*end_ ='\0';
}


void Binary::truncate(size_t position)
{
	if(position > length())
		return;

	makeUnique();
	end_ = begin_ + std::min(position, length());
	*end_ ='\0';
}


Binary Binary::simplified() const
{
	if(isEmpty())
		return Binary();

	Binary result = Binary::uninitialized(length());
	char* current = begin_;
	char* end = end_;
	char* output = result.begin_;

	while(::isspace(*(end - 1)))
		end--;

	while(true) {
		while(current != end && ::isspace(*current))
			current++;

		while(current != end && !::isspace(*current))
			*output++ = *current++;

		if(current != end) {
			*output++ = ' ';
		}
		else {
			break;
		}
	}

	*end = '\0';
	result.end_ = end;
	return result;
}


Binary Binary::trimmed() const
{
	if(isEmpty())
		return Binary();

	char* begin = begin_;
	char* end = end_;

	while(::isspace(*begin))
		begin++;

	while(::isspace(*(end - 1)))
		end--;

	if(begin == end)
		return Binary();

	Binary result = Binary::uninitialized(end - begin);
	result.end_ = std::copy(begin, end, result.begin_);

	*end = '\0';
	return result;
}


Binary Binary::toHex(bool upperCase) const
{
	if(isEmpty())
		return Binary();

	Binary result = Binary::uninitialized(length() * 2);
	char a = upperCase ? 'A' : 'a';
	char* current = begin_;
	char* output = result.begin_;

	while(current != end_) {
		int nibble = (*current >> 4) & 0x0F;
		if(nibble <= 9) {
			*output = '0' + nibble;
		}
		else {
			*output = a + nibble - 10;
		}

		output++;

		nibble = *current & 0x0F;
		if(nibble <= 9) {
			*output = '0' + nibble;
		}
		else {
			*output = a + nibble - 10;
		}

		output++;
		current++;
	}

	return result;
}


Binary Binary::fromHex(const char* string, size_t size)
{
	if(size == kNoPos)
		size = std::char_traits<char>::length(string);

	if(size == 0)
		return Binary();

	Binary result = Binary::uninitialized((size + 1) / 2);
	char* output = result.begin_;
	const char* current = string;

	*output = 0x00;
	bool isOdd = size % 2 == 0;

	while(current != string + size) {
		int ch = *current;

		if(ch >= '0' && ch <= '9') {
			ch = ch - '0';
		}
		else if(ch >= 'a' && ch <= 'f') {
			ch = ch - 'a' + 10;
		}
		else if(ch >= 'A' && ch <= 'F') {
			ch = ch - 'A' + 10;
		}
		else {
			return Binary();
		}

		if(isOdd) {
			*output = ch << 4;
		}
		else {
			*output |= ch;
			output++;
		}

		current++;
		isOdd ^= true;
	}

	return result;
}


Binary Binary::fromHex(const Binary& array)
{
	return fromHex(array.begin_, array.length());
}


Binary Binary::fromRawData(const char* data, size_t size)
{
	if(size == kNoPos)
		size = std::char_traits<char>::length(data);

	Binary result;
	result.buffer_ = Buffer::allocate(size);
	result.buffer_->data = const_cast<char*>(data);
	result.buffer_->capacity = 0;
	result.begin_ = result.buffer_->data;
	result.end_ = result.begin_ + size;

	return result;
}


char* Binary::data()
{
	makeUnique();
	return begin_;
}


const char* Binary::data() const
{
	return begin_;
}


const char* Binary::constData() const
{
	return begin_;
}


Binary& Binary::append(const char* string, size_t size)
{
	if(size == kNoPos)
		size = std::char_traits<char>::length(string);

	size_t newLength = length() + size;

	if(!buffer_->isUnique() || spaceAtEnd() < size) {
		Buffer* newBuffer = Buffer::allocate(newLength);
		char* newBegin = newBuffer->beginFor(newLength);

		char* pos = std::copy(begin_, end_, newBegin);
		end_ = std::copy(string, string + size, pos);
		begin_ = newBegin;

		buffer_->release();
		buffer_ = newBuffer;
	}
	else {
		end_ = std::copy(string, string + size, end_);
	}

	*end_ = '\0';
	return *this;
}


Binary& Binary::append(const Binary &other)
{
	return append(other.begin_, other.length());
}


Binary& Binary::append(char ch)
{
	return append(&ch, 1);
}


Binary& Binary::prepend(const char* string, size_t size)
{
	if(size == kNoPos)
		size = std::char_traits<char>::length(string);

	size_t newLength = length() + size;

	if(!buffer_->isUnique() || spaceAtBegin() < size) {
		Buffer* newBuffer = Buffer::allocate(newLength);
		char* newBegin = newBuffer->beginFor(newLength);

		char* pos = std::copy(string, string + size, newBegin);
		end_ = std::copy(begin_, end_, pos);
		begin_ = newBegin;

		buffer_->release();
		buffer_ = newBuffer;
	}
	else {
		begin_ = std::copy_backward(string, string + size, begin_);
	}

	*end_ = '\0';
	return *this;
}


Binary& Binary::prepend(const Binary &other)
{
	return prepend(other.begin_, other.length());
}


Binary& Binary::prepend(char ch)
{
	return prepend(&ch, 1);
}


Binary& Binary::insert(size_t position, const char* string, size_t size)
{
	if(position == 0) {
		return prepend(string, size);
	}
	else if(position >= length()) {
		return append(string, size);
	}

	if(size == kNoPos)
		size = std::char_traits<char>::length(string);

	size_t newLength = length() + size;

	if(!buffer_->isUnique() || newLength > length() + spaceAtEnd()) {
		Buffer* newBuffer = Buffer::allocate(newLength);
		char* newBegin = newBuffer->beginFor(newLength);

		char* pos = std::copy(begin_, begin_ + position, newBegin);
		pos = std::copy(string, string + size, pos);
		end_ = std::copy(begin_ + position, end_, pos);
		begin_ = newBegin;

		buffer_->release();
		buffer_ = newBuffer;
	}
	else if(position < length() / 2 && spaceAtBegin() > size) {
		std::move(begin_, begin_ + position, begin_ - size);
		std::copy(string, string + size, begin_ - size + position);
		begin_ -= size;
	}
	else {
		std::move_backward(begin_ + position, end_, end_ + size);
		std::copy(string, string + size, begin_ + position);
		end_ += size;
	}

	*end_ = '\0';
	return *this;
}


Binary& Binary::insert(size_t position, const Binary& other)
{
	return insert(position, other.begin_, other.length());
}


Binary& Binary::insert(size_t position, char ch)
{
	return insert(position, &ch, 1);
}


Binary& Binary::remove(size_t position, size_t count)
{
	if(position >= length() || count == 0)
		return *this;

	count = std::min(count, length() - position);
	size_t newLength = length() - count;

	if(!buffer_->isUnique()) {
		Buffer* newBuffer = Buffer::allocate(newLength);
		char* newBegin = newBuffer->beginFor(newLength);

		std::copy(begin_, begin_ + position, newBegin);
		std::copy(begin_ + position + count, end_, newBegin + position);

		begin_ = newBegin;
		end_ = newBegin + newLength;

		buffer_->release();
		buffer_ = newBuffer;
	}
	else if(newLength == 0) {
		begin_ = buffer_->beginFor(0);
		end_ = begin_;
	}
	else if(position == 0) {
		begin_ += count;
	}
	else if(position + count == length()) {
		end_ -= count;
	}
	else if(position < length() / 2) {
		std::move_backward(begin_, begin_ + position, begin_ + position + count);
		begin_ += count;
	}
	else {
		std::move(begin_ + position + count, end_, begin_ + position);
		end_ -= count;
	}

	*end_ = '\0';
	return *this;
}


Binary& Binary::remove(const char* string, size_t size)
{
	if(size == kNoPos)
		size = std::char_traits<char>::length(string);

	if(!buffer_->isUnique()) {
		Buffer* newBuffer = Buffer::allocate(length());
		char* newBegin = newBuffer->beginFor(length());

		char* begin = begin_;
		char* end = begin_;
		char* output = newBegin;
		size_t pos = 0;

		while(end != end_) {
			pos = indexOf(string, size, pos);
			end = (pos == kNoPos) ? end_ : begin_ + pos;

			output = std::copy(begin, end, output);
			begin = end + size;
			pos += size;
		}

		begin_ = newBegin;
		end_ = output;

		buffer_->release();
		buffer_ = newBuffer;
	}
	else {
		size_t end = std::min(indexOf(string, size), length());
		char* output = begin_ + end;

		while(end != length()) {
			size_t begin = end + size;
			end = std::min(indexOf(string, size, begin), length());
			output = std::move(begin_ + begin, begin_ + end, output);
		}

		end_ = output;
	}

	if(isEmpty()) {
		begin_ = buffer_->beginFor(buffer_->capacity);
		end_ = begin_;
	}

	*end_ = '\0';
	return *this;
}


Binary& Binary::remove(const Binary& other)
{
	return remove(other.begin_, other.length());
}


Binary& Binary::remove(char ch)
{
	return remove(&ch, 1);
}


Binary& Binary::replace(char before, const char* after, size_t afterSize)
{
	return replace(&before, after, 1, afterSize);
}


Binary& Binary::replace(char before, const Binary& after)
{
	return replace(&before, after.begin_, 1, after.length());
}


Binary& Binary::replace(char before, char after)
{
	return replace(&before, &after, 1, 1);
}


Binary& Binary::replace(const char* before, const char* after, size_t beforeSize,
		size_t afterSize)
{
	if(beforeSize == kNoPos)
		beforeSize = std::char_traits<char>::length(before);

	if(afterSize == kNoPos)
		afterSize = std::char_traits<char>::length(after);

	std::vector<size_t> offsets;
	size_t pos = 0;

	while(pos != kNoPos) {
		pos = indexOf(before, beforeSize, pos);
		if(pos != kNoPos) {
			offsets.push_back(pos);
			pos += beforeSize;
		}
	}

	if(offsets.empty())
		return *this;

	int delta = (afterSize - beforeSize) * offsets.size();
	size_t inc = (delta > 0) ? delta : 0;
	size_t oldLength = length();
	size_t newLength = oldLength + delta;

	if(!buffer_->isUnique() || spaceAtEnd() < inc) {
		Buffer* newBuffer = Buffer::allocate(newLength);
		char* newBegin = newBuffer->beginFor(newLength);

		char* begin = begin_;
		char* end;
		char* output = newBegin;

		for(size_t i = 0; i < offsets.size(); ++i) {
			end = begin_ + offsets[i];
			output = std::copy(begin, end, output);
			output = std::copy(after, after + afterSize, output);
			begin = end + beforeSize;
		}

		if(begin != end_)
			std::copy(begin, end_, output);

		begin_ = newBegin;
		end_ = newBegin + newLength;

		buffer_->release();
		buffer_ = newBuffer;
	}
	else if(delta > 0) {
		char* begin;
		char* end = end_;
		end_ = end + delta;

		for(int i = offsets.size() - 1; i >= 0; --i) {
			begin = begin_ + offsets[i] + beforeSize;
			end = std::move_backward(begin, end, end + delta);
			std::copy(after, after + afterSize, end - afterSize);
			end = begin - beforeSize;
			delta -= afterSize - beforeSize;
		}
	}
	else if(delta < 0) {
		char* begin = begin_;
		char* end = begin_ + offsets[0];
		char* output = end;

		for(size_t i = 1; i < offsets.size(); ++i) {
			output = std::copy(after, after + afterSize, output);

			begin = end + beforeSize;
			end = begin_ + offsets[i];
			output = std::move(begin, end, output);
		}

		if(begin != end_) {
			output = std::copy(after, after + afterSize, output);
			begin = end + beforeSize;
			end_ = std::move(begin, end_, output);
		}
	}
	else {
		for(size_t offset : offsets)
			std::copy(after, after + afterSize, begin_ + offset);
	}

	if(isEmpty()) {
		begin_ = buffer_->beginFor(buffer_->capacity);
		end_ = begin_;
	}

	*end_ = '\0';
	return *this;
}


Binary& Binary::replace(const Binary& before, const Binary& after)
{
	return replace(before.begin_, after.begin_, before.length(), after.length());
}


Binary& Binary::replace(const char* before, char after, size_t beforeSize)
{
	return replace(before, &after, beforeSize, 1);
}


Binary& Binary::replace(size_t position, size_t count, const char* after,
		size_t afterSize)
{
	if(afterSize == kNoPos)
		afterSize = std::char_traits<char>::length(after);

	if(position == 0 && count == length()) {
		*this = Binary(after, afterSize);
		return *this;
	}

	if(position >= length() || count == 0)
		return insert(position, after, afterSize);

	if(afterSize == 0)
		return remove(position, count);

	size_t oldLength = length();
	position = std::min(position, oldLength);
	count = std::min(count, oldLength - position);
	int delta = afterSize - count;
	size_t newLength = oldLength + delta;

	if(!buffer_->isUnique() || newLength > oldLength + spaceAtEnd()) {
		Buffer* newBuffer = Buffer::allocate(newLength);
		char* newBegin = newBuffer->beginFor(newLength);

		std::copy(begin_, begin_ + position, newBegin);
		std::copy(after, after + afterSize, newBegin + position);
		std::copy(begin_ + position + count, end_,
				newBegin + position + afterSize);

		begin_ = newBegin;
		end_ = newBegin + newLength;

		buffer_->release();
		buffer_ = newBuffer;
	}
	else if(position < oldLength / 2) {
		if(delta > 0) {
			std::move(begin_, begin_ + position, begin_ - delta);
		}
		else if(delta < 0) {
			std::move_backward(begin_, begin_ + position, begin_ - delta + position);
		}

		std::copy(after, after + afterSize, begin_ - delta + position);
		begin_ -= delta;
	}
	else {
		if(delta > 0) {
			std::move_backward(begin_ + position + count, end_, end_ + delta);
		}
		else if(delta < 0) {
			std::move(begin_ + position + count, end_, begin_ + position + count + delta);
		}

		std::copy(after, after + afterSize, begin_ + position);
		end_ += delta;
	}

	if(isEmpty()) {
		begin_ = buffer_->beginFor(buffer_->capacity);
		end_ = begin_;
	}

	*end_ = '\0';
	return *this;
}


Binary& Binary::replace(size_t position, size_t count, const Binary& after)
{
	return replace(position, count, after.begin_, after.length());
}


Binary& Binary::replace(size_t position, size_t count, char after)
{
	return replace(position, count, &after, 1);
}


size_t Binary::indexOf(const char* string, size_t size, size_t from) const
{
	if(size == 0)
		return from;

	if(size == 1)
		return indexOf(string[0], from);

	if(size == kNoPos)
		size = std::char_traits<char>::length(string);

	if(isEmpty() || from >= length() || (size + from) > length())
		return kNoPos;

	if(length() >= 512 && size > 5)
		return boyerMooreSearch(string, size, from);

	return hashingSearch(string, size, from);
}


size_t Binary::indexOf(const Binary& other, size_t from) const
{
	return indexOf(other.begin_, other.length(), from);
}


size_t Binary::indexOf(char ch, size_t from) const
{
	const char* result = std::find(begin_ + from, end_, ch);
	if(result != end_)
		return result - begin_;

	return kNoPos;
}


size_t Binary::lastIndexOf(const char* string, size_t size, size_t from) const
{
	if(size == 0)
		return from;

	if(size == 1)
		return lastIndexOf(string[0], from);

	if(size == kNoPos)
		size = std::char_traits<char>::length(string);

	if(isEmpty() || (length() - from) < size)
		return kNoPos;

	if(length() >= 512 && size > 5)
		return boyerMooreReverseSearch(string, size, from);

	return hashingReverseSearch(string, size, from);
}


size_t Binary::lastIndexOf(const Binary& other, size_t from) const
{
	return lastIndexOf(other.begin_, other.length(), from);
}


size_t Binary::lastIndexOf(char ch, size_t from) const
{
	typedef std::reverse_iterator<const char*> Iterator;

	size_t offset = length() - from - 1;
	Iterator rbegin(end_);
	Iterator rend(begin_);
	Iterator result = std::find(rbegin + offset, rend, ch);
	if(result != rend)
		return result.base() - begin_ - 1;

	return kNoPos;
}


Binary::Binary(const Binary& other, size_t position, size_t count) :
	buffer_(other.buffer_),
	begin_(other.begin_ + position),
	end_(begin_ + count)
{
	buffer_->acquire();
}


size_t Binary::hashingSearch(const char* string, size_t size, size_t from) const
{
	u32 needleHash = 0;
	u32 haystackHash = 0;
	size_t pos = from;

	while(pos < size + from) {
		needleHash = (needleHash << 1) + string[pos-from];
		haystackHash = (haystackHash << 1) + begin_[pos];
		++pos;
	}

	while(pos <= length()) {
		if(needleHash == haystackHash) {
			size_t offset = pos - size;
			if(std::equal(string, string + size, begin_ + offset))
				return offset;
		}

		if(size - 1 < Limits<u32>::bitCount())
			haystackHash -= begin_[pos-size] << (size - 1);

		haystackHash = (haystackHash << 1) + begin_[pos];
		++pos;
	}

	return kNoPos;
}


size_t Binary::hashingReverseSearch(const char* string, size_t size, size_t from) const
{
	u32 needleHash = 0;
	u32 haystackHash = 0;

	if(from >= length())
		from = length() - 1;

	size_t pos = from;

	while(pos > from - size) {
		needleHash = (needleHash << 1) + string[size-(from-pos)-1];
		haystackHash = (haystackHash << 1) + begin_[pos];
		--pos;
	}

	while(pos + 1 != kNoPos) {
		if(needleHash == haystackHash) {
			size_t offset = pos + 1;
			if(std::equal(string, string + size, begin_ + offset))
				return offset;
		}

		if(size - 1 < Limits<u32>::bitCount())
			haystackHash -= begin_[pos+size] << (size - 1);

		haystackHash = (haystackHash << 1) + begin_[pos];
		--pos;
	}

	return kNoPos;
}


size_t Binary::boyerMooreSearch(const char* string, size_t size, size_t from) const
{
	const size_t kMaxSkip = Limits<u8>::max();
	const size_t kTableSize = kMaxSkip + 1;
	u8 skipTable[kTableSize];

	size_t len = std::min(size, kMaxSkip);
	std::fill(skipTable, skipTable + kTableSize, len);

	const char* p = string + size - len;
	while(--len) {
		skipTable[*reinterpret_cast<const u8*>(p)] = len;
		p++;
	}

	p = begin_ + from + size - 1;
	while(p < end_) {
		const char* start = p - size + 1;
		if((string + size)[-1] == *p && std::equal(start, p, string))
			return start - begin_;

		p += skipTable[*reinterpret_cast<const u8*>(p)];
	}

	return kNoPos;
}


size_t Binary::boyerMooreReverseSearch(const char* string, size_t size, size_t from) const
{
	const size_t kMaxSkip = Limits<u8>::max();
	const size_t kTableSize = kMaxSkip + 1;
	u8 skipTable[kTableSize];

	size_t skip = std::min(size, kMaxSkip);
	std::fill(skipTable, skipTable + kTableSize, skip);

	const char* p = string + skip - 1;
	while(--skip) {
		skipTable[*reinterpret_cast<const u8*>(p)] = skip;
		p--;
	}

	p = begin_ + from;
	while(p + size >= begin_ + size) {
		const char* end = p + size - 1;
		if(string[0] == *p && std::equal(p, end, string))
			return p - begin_;

		p -= skipTable[*reinterpret_cast<const u8*>(p)];
	}

	return kNoPos;
}


Binary Binary::toUpper() const
{
	Binary result = Binary::uninitialized(length());

	std::transform(begin_, end_, result.begin_, ::toupper);
	return result;
}


Binary Binary::toLower() const
{
	Binary result = Binary::uninitialized(length());

	std::transform(begin_, end_, result.begin_, ::tolower);
	return result;
}


int Binary::resize(size_t newLength)
{
	int delta = newLength - length();

	if(delta != 0) {
		if(!buffer_->isUnique() || spaceAtEnd() < newLength) {
			Buffer* newBuffer = Buffer::allocate(newLength);
			char* newBegin = newBuffer->beginFor(newLength);

			end_ = std::copy(begin_, end_, newBegin);
			begin_ = newBegin;

			buffer_->release();
			buffer_ = newBuffer;
		}

		end_ += delta;
		*end_ = '\0';
	}

	return delta;
}


int Binary::resize(size_t newLength, char value)
{
	int delta = resize(newLength);

	if(delta > 0) {
		std::fill(end_ - delta, end_, value);
		*end_ = '\0';
	}

	return delta;
}


Binary::Buffer* Binary::sharedNull()
{
	if(!sharedNull_)
		sharedNull_.reset(new Buffer());

	return sharedNull_.get();
}


Binary Binary::uninitialized(size_t size)
{
	Binary result = Binary(Buffer::allocate(size));
	result.begin_ = result.buffer_->beginFor(size);
	result.end_ = result.begin_ + size;
	return result;
}


Binary::Binary(Buffer* buffer) :
	buffer_(buffer),
	begin_(nullptr),
	end_(nullptr)
{
}


void Binary::makeUnique()
{
	if(!buffer_->isUnique()) {
		Buffer* newBuffer = Buffer::allocate(length());
		char* newBegin = newBuffer->beginFor(length());

		end_ = std::copy(begin_, end_, newBegin);
		*end_ = '\0';
		end_++;

		begin_ = newBegin;

		buffer_->release();
		buffer_ = newBuffer;
	}
}


Binary::operator const char*() const
{
	return begin_;
}


Binary::operator const void*() const
{
	return begin_;
}


bool operator==(const char* string, const Binary& array)
{
	return array == string;
}


bool operator!=(const char* string, const Binary& array)
{
	return array != string;
}


bool operator<(const char* string, const Binary& array)
{
	return !(array >= string);
}


bool operator>(const char* string, const Binary& array)
{
	return !(array <= string);
}


bool operator<=(const char* string, const Binary& array)
{
	return !(array > string);
}


bool operator>=(const char* string, const Binary& array)
{
	return !(array < string);
}


Binary operator+(const Binary& a1, const Binary& a2)
{
	Binary result = a1;
	return result += a2;
}


Binary operator+(const char* string, const Binary& array)
{
	return Binary(string) += array;
}


Binary operator+(const Binary& array, const char* string)
{
	return Binary(array) += string;
}


Binary operator+(char ch, const Binary& array)
{
	return Binary(ch) += array;
}


Binary operator+(const Binary& array, char ch)
{
	return Binary(array) += ch;
}


} // namespace Tech
