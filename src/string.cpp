#include <tech/string.h>

#include <algorithm>
#include <atomic>
#include <tech/utils.h>


namespace Tech {


struct String::Buffer {
	std::atomic<uint> rc;

	// capacity содержит значение размера выделенного буфера, а в случае, если
	// используется внешний буфер (String сконструирован через вызов fromRawData()),
	// значение capacity устанавливается в 0. Поэтому во всех функциях, которые
	// используют capacity, требуется учитывать этот случай.
	size_t capacity;
	ch16* data;

	static const size_t kMinCapacity = 16;

	// Используется только для создания shared null экземпляра. Для защиты этого объекта
	// от изменения и удаления, счетчик ссылок устанавливается в единицу, несмотря на то,
	// что ссылок на объект еще не существует.
	Buffer() :
		rc(1),
		capacity(0),
		data(const_cast<ch16*>(u""))
	{
	}

	// Предполагается, что вызывающий код становится владельцем нового буфера, поэтому
	// счетчик ссылок изначально устанавливается в единицу, чтобы не выполнять лишнюю
	// атомарную операцию при acquire().
	Buffer(size_t cap) :
		rc(1),
		capacity(cap),
		data(reinterpret_cast<ch16*>(this + 1))
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
	// fromRawData().
	ch16* beginFor(size_t length) const
	{
		return data + (capacity - length) / 2;
	}

	static Buffer* allocate(size_t length)
	{
		size_t capacity = capacityFor(length);
		void* result = operator new(sizeof(Buffer) + sizeof(ch16) * capacity);

		return new (result) Buffer(capacity);
	}

	static size_t capacityFor(size_t length)
	{
		if(length <= kMinCapacity / 2)
			return kMinCapacity;

		return ceilToPowerOfTwo(length * 2);
	}
};


const size_t String::Buffer::kMinCapacity;
const size_t String::kNoPos;
std::unique_ptr<String::Buffer> String::sharedNull_;


String::String() :
	buffer_(sharedNull()),
	begin_(nullptr),
	end_(nullptr)
{
	buffer_->acquire();
}


String::String(const char* string) :
	String()
{
	*this = fromUtf8(string);
}


String::String(const char* string, size_t size) :
	String()
{
	*this = fromUtf8(string, size);
}


String::String(const ch16* string)
{
	size_t size = std::char_traits<ch16>::length(string);

	buffer_ = Buffer::allocate(size);
	begin_ = buffer_->beginFor(size);
	end_ = std::copy(string, string + size, begin_);
}


String::String(const ch16* string, size_t size)
{
	buffer_ = Buffer::allocate(size);
	begin_ = buffer_->beginFor(size);
	end_ = std::copy(string, string + size, begin_);
}


String::String(const ByteArray& ba) :
	String()
{
	*this = fromUtf8(ba.data(), ba.length());
}


String::String(const std::string& string) :
	String(string.data(), string.size())
{
}


String::String(const std::u16string& string) :
	String(string.data(), string.size())
{
}


String::String(const String& other)
{
	other.buffer_->acquire();
	buffer_ = other.buffer_;
	begin_ = other.begin_;
	end_ = other.end_;
}


String::String(Char ch)
{
	buffer_ = Buffer::allocate(1);
	begin_ = buffer_->beginFor(1);
	*begin_ = ch.unicode();
	end_ = begin_ + 1;
}


String::String(size_t count, Char ch)
{
	buffer_ = Buffer::allocate(count);
	begin_ = buffer_->beginFor(count);
	end_ = begin_ + count;
	std::fill(begin_, end_, ch.unicode());
}


String::~String()
{
	buffer_->release();
}


bool String::isNull() const
{
	return buffer_ == sharedNull();
}


bool String::isEmpty() const
{
	return begin_ == end_;
}


size_t String::length() const
{
	return end_ - begin_;
}


size_t String::spaceAtBegin() const
{
	return begin_ - buffer_->data;
}


size_t String::spaceAtEnd() const
{
	if(buffer_->capacity)
		return buffer_->capacity - (end_ - buffer_->data);

	return 0;
}


void String::reserveAtBegin(size_t size)
{
	if(!buffer_->isUnique() || spaceAtBegin() < size) {
		size_t totalSize = length() + size;
		Buffer* newBuffer = Buffer::allocate(totalSize);
		ch16* newBegin = newBuffer->beginFor(totalSize);

		end_ = std::copy(begin_, end_, newBegin);
		begin_ = newBegin;

		buffer_->release();
		buffer_ = newBuffer;
	}
}


void String::reserveAtEnd(size_t size)
{
	if(!buffer_->isUnique() || spaceAtEnd() < size) {
		size_t totalSize = length() + size;
		Buffer* newBuffer = Buffer::allocate(totalSize);
		ch16* newBegin = newBuffer->beginFor(totalSize);

		end_ = std::copy(begin_, end_, newBegin);
		begin_ = newBegin;

		buffer_->release();
		buffer_ = newBuffer;
	}
}


Char* String::data()
{
	makeUnique();
	return reinterpret_cast<Char*>(begin_);
}


const Char* String::data() const
{
	return reinterpret_cast<Char*>(begin_);
}


const Char* String::constData() const
{
	return reinterpret_cast<Char*>(begin_);
}


bool String::startsWith(const String& string) const
{
	if(string.length() > length())
		return false;

	return std::equal(string.begin_, string.end_, begin_);
}


bool String::startsWith(Char ch) const
{
	if(isEmpty())
		return false;

	return *begin_ == ch.unicode();
}


bool String::endsWith(const String& string) const
{
	if(string.length() > length())
		return false;

	return std::equal(string.begin_, string.end_, end_ - string.length());
}


bool String::endsWith(Char ch) const
{
	if(isEmpty())
		return false;

	return *(end_ - 1) == ch.unicode();
}


size_t String::indexOf(const String& string, size_t from) const
{
	return indexOf(string.begin_, string.length(), from);
}


size_t String::indexOf(Char ch, size_t from) const
{
	const ch16* result = std::find(begin_ + from, end_, ch.unicode());
	if(result != end_)
		return result - begin_;

	return kNoPos;
}


size_t String::indexOf(const ch16* data, size_t size, size_t from) const
{
	if(size == 0)
		return from;

	if(size == 1)
		return indexOf(*data, from);

	if(isEmpty() || from >= length() || (size + from) > length())
		return kNoPos;

	if(length() >= 512 && size > 5)
		return boyerMooreSearch(data, size, from);

	return hashingSearch(data, size, from);
}


size_t String::lastIndexOf(const String& string, size_t from) const
{
	return lastIndexOf(string.begin_, string.length(), from);
}


size_t String::lastIndexOf(Char ch, size_t from) const
{
	typedef std::reverse_iterator<const ch16*> Iterator;

	size_t offset = length() - from - 1;
	Iterator rbegin(end_);
	Iterator rend(begin_);
	Iterator result = std::find(rbegin + offset, rend, ch.unicode());
	if(result != rend)
		return result.base() - begin_ - 1;

	return kNoPos;
}


size_t String::lastIndexOf(const ch16* data, size_t size, size_t from) const
{
	if(size == 0)
		return from;

	if(size == 1)
		return lastIndexOf(*data, from);

	if(from == kNoPos) {
		// Если length() == 0, то значение from не изменится (останется равно NoPos).
		from = length() - 1;
	}

	if(isEmpty() || from >= length() || (from <= size))
		return kNoPos;

	if(length() >= 512 && size > 5)
		return boyerMooreReverseSearch(data, size, from);

	return hashingReverseSearch(data, size, from);
}


size_t String::hashingSearch(const ch16* data, size_t size, size_t from) const
{
	u64 needleHash = 0;
	u64 haystackHash = 0;
	size_t pos = from;

	while(pos < size + from) {
		needleHash = (needleHash << 1) + data[pos-from];
		haystackHash = (haystackHash << 1) + begin_[pos];
		++pos;
	}

	while(pos <= length()) {
		if(needleHash == haystackHash) {
			size_t offset = pos - size;
			if(std::equal(data, data + size, begin_ + offset))
				return offset;
		}

		if(size - 1 < Limits<u64>::bitCount())
			haystackHash -= begin_[pos-size] << (size - 1);

		haystackHash = (haystackHash << 1) + begin_[pos];
		++pos;
	}

	return kNoPos;
}


size_t String::hashingReverseSearch(const ch16* data, size_t size, size_t from) const
{
	u64 needleHash = 0;
	u64 haystackHash = 0;
	size_t pos = from;

	while(pos > from - size) {
		needleHash = (needleHash << 1) + data[size-(from-pos)-1];
		haystackHash = (haystackHash << 1) + begin_[pos];
		--pos;
	}

	while(pos + 1 != kNoPos) {
		if(needleHash == haystackHash) {
			size_t offset = pos + 1;
			if(std::equal(data, data + size, begin_ + offset))
				return offset;
		}

		if(size - 1 < Limits<u64>::bitCount())
			haystackHash -= begin_[pos+size] << (size - 1);

		haystackHash = (haystackHash << 1) + begin_[pos];
		--pos;
	}

	return kNoPos;
}


size_t String::boyerMooreSearch(const ch16* data, size_t size, size_t from) const
{
	const size_t kMaxSkip = Limits<u8>::max();
	const size_t kTableSize = kMaxSkip + 1;
	u8 skipTable[kTableSize];

	size_t skip = std::min(size, kMaxSkip);
	std::fill(skipTable, skipTable + kTableSize, skip);

	const ch16* p = data + size - skip;
	while(--skip) {
		skipTable[*p & 0x00FF] = skip;
		p++;
	}

	p = begin_ + from + size - 1;
	while(p < end_) {
		const ch16* start = p - size + 1;
		if(std::equal(start, p, data))
			return start - begin_;

		p += skipTable[*p];
	}

	return kNoPos;
}


size_t String::boyerMooreReverseSearch(const ch16* data, size_t size, size_t from) const
{
	const size_t kMaxSkip = Limits<u8>::max();
	const size_t kTableSize = kMaxSkip + 1;
	u8 skipTable[kTableSize];

	size_t skip = std::min(size, kMaxSkip);
	std::fill(skipTable, skipTable + kTableSize, skip);

	const ch16* p = data + skip - 1;
	while(--skip) {
		skipTable[*p & 0x00FF] = skip;
		p--;
	}

	p = begin_ + from;
	while(p + size >= begin_ + size) {
		const ch16* end = p + size - 1;
		if(std::equal(p, end, data))
			return p - begin_;

		p -= skipTable[*p];
	}

	return kNoPos;
}


void String::clear()
{
	if(!buffer_->isUnique()) {
		buffer_->release();
		buffer_ = Buffer::allocate(0);
	}

	begin_ = buffer_->beginFor(0);
	end_ = begin_;
}


void String::swap(String& other)
{
	std::swap(buffer_, other.buffer_);
	std::swap(begin_, other.begin_);
	std::swap(end_, other.end_);
}


String& String::append(const String& string)
{
	return append(string.begin_, string.length());
}


String& String::append(const char* string)
{
	return append(fromUtf8(string));
}


String& String::append(const Char* data, size_t count)
{
	return append(reinterpret_cast<const ch16*>(data), count);
}


String& String::append(Char ch)
{
	return append(reinterpret_cast<ch16*>(&ch), 1);
}


String& String::append(const ch16* data, size_t size)
{
	size_t newLength = length() + size;

	if(!buffer_->isUnique() || spaceAtEnd() < size) {
		Buffer* newBuffer = Buffer::allocate(newLength);
		ch16* newBegin = newBuffer->beginFor(newLength);

		ch16* pos = std::copy(begin_, end_, newBegin);
		end_ = std::copy(data, data + size, pos);
		begin_ = newBegin;

		buffer_->release();
		buffer_ = newBuffer;
	}
	else {
		end_ = std::copy(data, data + size, end_);
	}

	return *this;
}


String& String::prepend(const String& string)
{
	return prepend(string.begin_, string.length());
}


String& String::prepend(const char* string)
{
	return prepend(fromUtf8(string));
}


String& String::prepend(Char ch)
{
	return prepend(reinterpret_cast<ch16*>(&ch), 1);
}


String& String::prepend(const ch16* data, size_t size)
{
	size_t newLength = length() + size;

	if(!buffer_->isUnique() || spaceAtBegin() < size) {
		Buffer* newBuffer = Buffer::allocate(newLength);
		ch16* newBegin = newBuffer->beginFor(newLength);

		ch16* pos = std::copy(data, data + size, newBegin);
		end_ = std::copy(begin_, end_, pos);
		begin_ = newBegin;

		buffer_->release();
		buffer_ = newBuffer;
	}
	else {
		begin_ = std::copy_backward(data, data + size, begin_);
	}

	return *this;
}


String& String::insert(size_t position, const String& string)
{
	return insert(position, string.begin_, string.length());
}


String& String::insert(size_t position, const char* string)
{
	return insert(position, fromUtf8(string));
}


String& String::insert(size_t position, Char ch)
{
	if(position == 0) {
		return prepend(ch);
	}
	else if(position >= length()) {
		return append(ch);
	}

	size_t newLength = length() + 1;

	if(!buffer_->isUnique() || newLength > length() + spaceAtEnd()) {
		Buffer* newBuffer = Buffer::allocate(newLength);
		ch16* newBegin = newBuffer->beginFor(newLength);

		ch16* pos = std::copy(begin_, begin_ + position, newBegin);
		*pos = ch.unicode();
		pos++;
		end_ = std::copy(begin_ + position, end_, pos);
		begin_ = newBegin;

		buffer_->release();
		buffer_ = newBuffer;
	}
	else if(position < length() / 2 && spaceAtBegin()) {
		std::move(begin_, begin_ + position, begin_ - 1);
		*begin_ = ch.unicode();
		begin_--;
	}
	else {
		std::move_backward(begin_ + position, end_, end_ + 1);
		*(begin_ + position) = ch.unicode();
		end_++;
	}

	return *this;
}


String& String::insert(size_t position, const ch16* data, size_t size)
{
	if(position == 0) {
		return prepend(data, size);
	}
	else if(position >= length()) {
		return append(data, size);
	}

	size_t newLength = length() + size;

	if(!buffer_->isUnique() || newLength > length() + spaceAtEnd()) {
		Buffer* newBuffer = Buffer::allocate(newLength);
		ch16* newBegin = newBuffer->beginFor(newLength);

		ch16* pos = std::copy(begin_, begin_ + position, newBegin);
		pos = std::copy(data, data + size, pos);
		end_ = std::copy(begin_ + position, end_, pos);
		begin_ = newBegin;

		buffer_->release();
		buffer_ = newBuffer;
	}
	else if(position < length() / 2 && spaceAtBegin() > size) {
		std::move(begin_, begin_ + position, begin_ - size);
		std::copy(data, data + size, begin_ - size + position);
		begin_ -= size;
	}
	else {
		std::move_backward(begin_ + position, end_, end_ + size);
		std::copy(data, data + size, begin_ + position);
		end_ += size;
	}

	return *this;
}


String& String::remove(size_t position, size_t count)
{
	if(position >= length() || count == 0)
		return *this;

	count = std::min(count, length() - position);
	size_t newLength = length() - count;

	if(!buffer_->isUnique()) {
		Buffer* newBuffer = Buffer::allocate(newLength);
		ch16* newBegin = newBuffer->beginFor(newLength);

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
		std::move_backward(begin_, begin_ + position,
				begin_ + position + count);

		begin_ += count;
	}
	else {
		std::move(begin_ + position + count, end_, begin_ + position);
		end_ -= count;
	}

	return *this;
}


String& String::remove(const String& string)
{
	return remove(string.begin_, string.length(), 0);
}


String& String::remove(Char ch)
{
	return remove(reinterpret_cast<ch16*>(&ch), 1, 0);
}


String& String::remove(const ch16* data, size_t size, int dummy)
{
	// dummy используется для разрешения неоднозначных перегрузок
	UNUSED(dummy);

	if(!size)
		return *this;

	if(!buffer_->isUnique()) {
		Buffer* newBuffer = Buffer::allocate(length());
		ch16* newBegin = newBuffer->beginFor(length());

		ch16* begin = begin_;
		ch16* end = begin_;
		ch16* output = newBegin;
		size_t pos = 0;

		while(end != end_) {
			pos = indexOf(data, size, pos);
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
		size_t end = std::min(indexOf(data, size), length());
		ch16* output = begin_ + end;

		while(end != length()) {
			size_t begin = end + size;
			end = std::min(indexOf(data, size, begin), length());
			output = std::move(begin_ + begin, begin_ + end, output);
		}

		end_ = output;
	}

	if(isEmpty()) {
		begin_ = buffer_->beginFor(buffer_->capacity);
		end_ = begin_;
	}

	return *this;
}


String& String::replace(const String& before, const String& after)
{
	return replace(before.begin_, before.length(), after.begin_,
			after.length());
}


String& String::replace(Char before, Char after)
{
	return replace(reinterpret_cast<ch16*>(&before), 1,
			reinterpret_cast<ch16*>(&after), 1);
}


String& String::replace(Char before, const String& after)
{
	return replace(reinterpret_cast<ch16*>(&before), 1,
			after.begin_, after.length());
}


String& String::replace(const String& before, Char after)
{
	return replace(before.begin_, before.length(),
			reinterpret_cast<ch16*>(&after), 1);
}


String& String::replace(size_t position, size_t count, const String& after)
{
	return replace(position, count, after.begin_, after.length());
}


String& String::replace(size_t position, size_t count, Char after)
{
	return replace(position, count, reinterpret_cast<ch16*>(&after), 1);
}


String& String::replace(size_t position, size_t count, const ch16* after,
		size_t afterSize)
{
	if(position == 0 && count == length()) {
		*this = after;
		return *this;
	}

	if(position >= length() || count == 0)
		return insert(position, after, afterSize);

	if(!afterSize)
		return remove(position, count);

	size_t oldLength = length();
	position = std::min(position, oldLength);
	count = std::min(count, oldLength - position);
	int delta = afterSize - count;
	size_t newLength = oldLength + delta;

	if(!buffer_->isUnique() || newLength > oldLength + spaceAtEnd()) {
		Buffer* newBuffer = Buffer::allocate(newLength);
		ch16* newBegin = newBuffer->beginFor(newLength);

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

	return *this;
}


String& String::replace(const ch16* before, size_t beforeSize, const ch16* after,
		size_t afterSize)
{
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
		ch16* newBegin = newBuffer->beginFor(newLength);

		ch16* begin = begin_;
		ch16* end;
		ch16* output = newBegin;

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
		ch16* begin;
		ch16* end = end_;
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
		ch16* begin = begin_;
		ch16* end = begin_ + offsets[0];
		ch16* output = end;

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

	return *this;
}


String& String::fill(Char ch)
{
	makeUnique();
	std::fill(begin_, end_, ch.unicode());
	return *this;
}


String String::left(size_t count) const
{
	count = std::min(count, length());
	return String(*this, 0, count);
}


String String::middle(size_t from, size_t count) const
{
	from = std::min(from, length());
	count = std::min(count, length() - from);
	return String(*this, from, count);
}


String String::right(size_t count) const
{
	count = std::min(count, length());
	size_t from = length() - count;
	return String(*this, from, count);
}


String String::leftJustified(size_t width, Char fill, bool truncate) const
{
	if(width >= length()) {
		String result = *this;
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


String String::middleJustified(size_t width, Char fill, bool truncate) const
{
	if(width >= length()) {
		String result = *this;
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


String String::rightJustified(size_t width, Char fill, bool truncate) const
{
	if(width >= length()) {
		String result = *this;
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


StringList String::split(Char sep, SplitBahavior behavior) const
{
	StringList result;

	ch16* begin = begin_;
	ch16* end = begin_;

	while(end != end_) {
		size_t position = std::distance(begin_, begin);

		end = std::find(begin, end_, sep.unicode());

		size_t count = std::distance(begin, end);
		begin = end + 1;

		if(count > 0 || behavior == kKeepEmptyParts)
			result.emplace_back(middle(position, count));
	}

	return result;
}


StringList String::split(const String& sep, SplitBahavior behavior) const
{
	StringList result;

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


void String::chop(size_t n)
{
	if(n == 0)
		return;

	makeUnique();
	end_ -= std::min(n, length());
}


void String::truncate(size_t position)
{
	if(position > length())
		return;

	makeUnique();
	end_ = begin_ + std::min(position, length());
}


String String::simplified() const
{
	if(isEmpty())
		return String();

	String result;
	Buffer* newBuffer = Buffer::allocate(length());
	ch16* newBegin = newBuffer->beginFor(length());
	result.begin_ = newBegin;
	result.end_ = newBegin;
	result.buffer_->release();
	result.buffer_ = newBuffer;

	ch16* current = begin_;
	ch16* end = end_;
	ch16* output = result.begin_;

	while(Char(*(end - 1)).isSpace())
		end--;

	while(true) {
		while(current != end && Char(*current).isSpace())
			current++;

		while(current != end && !Char(*current).isSpace())
			*output++ = *current++;

		if(current != end) {
			*output++ = ' ';
		}
		else {
			break;
		}
	}

	result.end_ = end;
	return result;
}


String String::trimmed() const
{
	if(isEmpty())
		return String();

	ch16* begin = begin_;
	ch16* end = end_;

	while(Char(*begin).isSpace())
		begin++;

	while(Char(*(end - 1)).isSpace())
		end--;

	if(begin == end)
		return String();

	return String(*this, begin - buffer_->data, end - begin);
}


ByteArray String::toUtf8() const
{
	ByteArray result;
	const char16_t* pos = begin_;

	while(pos != end_) {
		char16_t word = *pos;
		char32_t code; // code point

		if(word < 0xD800 || word > 0xDFFF) {
			code = static_cast<char32_t>(word);
		}
		else if(word >= 0xDC00) {
			return ByteArray();
		}
		else {
			code = (word & 0x03FF) << 10;
			pos++;
			if(pos == end_)
				return ByteArray();

			word = *pos;
			if(word < 0xDC00 || word > 0xDFFF) {
				return ByteArray();
			}
			else {
				code |= word & 0x03FF;
				code += 0x10000;
			}
		}

		if(code < 0x0080) {
			result += code;
		}
		else if(code < 0x0800) {
			result += 0xC0 | (code >> 6);
			result += 0x80 | (code & 0x3F);
		}
		else if(code < 10000) {
			result += 0xE0 | (code >> 12);
			result += 0x80 | ((code >> 6) & 0x3F);
			result += 0x80 | (code & 0x3F);
		}
		else if(code < 200000) {
			result += 0xF0 | (code >> 18);
			result += 0x80 | ((code >> 12) & 0x3F);
			result += 0x80 | ((code >> 6) & 0x3F);
			result += 0x80 | (code & 0x3F);
		}
		else if(code < 40000000) {
			result += 0xF8 | (code >> 24);
			result += 0x80 | ((code >> 18) & 0x3F);
			result += 0x80 | ((code >> 12) & 0x3F);
			result += 0x80 | ((code >> 6) & 0x3F);
			result += 0x80 | (code & 0x3F);
		}
		else {
			result += 0xFC | (code >> 30);
			result += 0x80 | ((code >> 24) & 0x3F);
			result += 0x80 | ((code >> 18) & 0x3F);
			result += 0x80 | ((code >> 12) & 0x3F);
			result += 0x80 | ((code >> 6) & 0x3F);
			result += 0x80 | (code & 0x3F);
		}

		pos++;
	}

	return result;
}


String String::fromUtf8(const char* string, size_t length)
{
	if(length == kNoPos)
		length = std::char_traits<char>::length(string);

	String result;
	size_t i = 0;

	while(i < length) {
		char32_t code; // code point
		uint count;
		uchar byte = string[i++];

		if(byte == 0x00) { // Проверка на конец строки при length == NoPos
			return result;
		}
		else if(byte <= 0x7F) {
			code = byte;
			count = 0;
		}
		else if(byte <= 0xBF) {
			return String();
		}
		else if(byte <= 0xDF)	{
			code = byte & 0x1F;
			count = 1;
		}
		else if(byte <= 0xEF)	{
			code = byte & 0x0F;
			count = 2;
		}
		else if(byte <= 0xF7)	{
			code = byte & 0x07;
			count = 3;
		}
		else {
			return String();
		}

		for(uint j = 0; j < count; ++j) {
			if(i == length)
				return String();

			byte = string[i++];
			if(byte < 0x80 || byte > 0xBF)
				return String();

			code <<= 6;
			code += byte & 0x3F;
		}

		if((code >= 0xD800 && code <= 0xDFFF) || code > 0x10FFFF)
			return String();

		if(code <= 0xFFFF) {
			result += static_cast<Char>(code);
		}
		else {
			code -= 0x10000;
			result += static_cast<Char>((code >> 10) + 0xD800);
			result += static_cast<Char>((code & 0x03FF) + 0xDC00);
		}
	}

	return result;
}


String String::fromRawData(const Char* unicode, size_t length)
{
	const ch16* data = reinterpret_cast<const ch16*>(unicode);

	if(length == kNoPos)
		length = std::char_traits<ch16>::length(data);

	String result;
	result.buffer_ = Buffer::allocate(length); // ???
	result.buffer_->data = const_cast<ch16*>(data);
	result.buffer_->capacity = 0;              // ???
	result.begin_ = result.buffer_->data;
	result.end_ = result.begin_ + length;

	return result;
}


String& String::operator=(const String& other)
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


String& String::operator=(Char ch)
{
	if(!buffer_->isUnique()) {
		buffer_->release();
		buffer_ = Buffer::allocate(1);
	}

	begin_ = buffer_->beginFor(1);
	*begin_ = ch.unicode();

	end_ = begin_ + 1;
	return *this;
}


bool String::operator==(const String& string) const
{
	if(buffer_ == string.buffer_)
		return true;

	if(length() != string.length())
		return false;

	return std::equal(begin_, end_, string.begin_);
}


bool String::operator!=(const String& string) const
{
	return !(*this == string);
}


bool String::operator<(const String& string) const
{
	return std::lexicographical_compare(begin_, end_, string.begin_, string.end_);
}


bool String::operator<=(const String& string) const
{
	auto func = [](ch16 c1, ch16 c2) -> bool { return c1 <= c2; };
	return std::lexicographical_compare(begin_, end_, string.begin_, string.end_, func);
}


bool String::operator>(const String& string) const
{
	auto func = [](ch16 c1, ch16 c2) -> bool { return c1 > c2; };
	return std::lexicographical_compare(begin_, end_, string.begin_, string.end_, func);
}


bool String::operator>=(const String& string) const
{
	auto func = [](ch16 c1, ch16 c2) -> bool { return c1 >= c2; };
	return std::lexicographical_compare(begin_, end_, string.begin_, string.end_, func);
}


String& String::operator+=(const String& string)
{
	return append(string);
}


String& String::operator+=(Char ch)
{
	return append(ch);
}


Char String::operator[](size_t position) const
{
	return begin_[position];
}


CharRef String::operator[](size_t position)
{
	return CharRef(*this, position);
}


String::Buffer* String::sharedNull()
{
	if(!sharedNull_)
		sharedNull_.reset(new Buffer());

	return sharedNull_.get();
}


String::String(const String& other, size_t position, size_t count) :
	buffer_(other.buffer_),
	begin_(other.begin_ + position),
	end_(begin_ + count)
{
	buffer_->acquire();
}


void String::makeUnique()
{
	if(!buffer_->isUnique()) {
		Buffer* newBuffer = Buffer::allocate(length());
		ch16* newBegin = newBuffer->beginFor(length());

		end_ = std::copy(begin_, end_, newBegin);
		begin_ = newBegin;

		buffer_->release();
		buffer_ = newBuffer;
	}
}


String operator+(const String& string1, const String& string2)
{
	String result = string1;
	return result += string2;
}


String operator+(Char ch, const String& string)
{
	return String(ch) += string;
}


String operator+(const String& string, Char ch)
{
	return String(string) += ch;
}


} // namespace Tech
