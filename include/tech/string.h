#ifndef TECH_STRING_H
#define TECH_STRING_H

#include <memory>
#include <string>
#include <vector>
#include <tech/binary.h>
#include <tech/char.h>


namespace Tech {


class CharRef;
class String;

using StringList = std::vector<String>;


class String final {
public:
	/**
	 * Перечисление используется в функции split() для указания необходимого действия над
	 * пустыми элементами.
	 */
	enum SplitBahavior {
		kKeepEmptyParts, ///< Все элементы включаются в результат.
		kSkipEmptyParts  ///< Пустые элементы не включаются в результат.
	};

	/**
	 * Используется в качестве особого значения, смысл которого зависит от контекста
	 * применения. Так например функции @c indexOf() возвращают @c kNoPos в случае, если
	 * не удалось найти искомую подстроку.
	 */
	static const size_t kNoPos = -1;

	/**
	 * Создает пустую нулевую строку.
	 */
	String();

	/**
	 * Создает строку из символов массива @p string. При этом выполняется глубокое
	 * копирование во внутренний буфер строки. Предполагается, что @p string - это
	 * завершенная нулем C-строка.
	 */
	String(const char* string);

	/**
	 * Создает строку из первых @p size символов массива @p string. При этом выполняется
	 * глубокое копирование во внутренний буфер строки.
	 */
	String(const char* string, size_t size);

	/**
	 * Создает строку из символов массива @p string. При этом выполняется глубокое
	 * копирование во внутренний буфер строки. Предполагается, что @p string - это
	 * завершенная нулем строка.
	 */
	String(const ch16* string);

	/**
	 * Создает строку из первых @p size символов массива @p string. При этом выполняется
	 * глубокое копирование во внутренний буфер строки.
	 */
	String(const ch16* string, size_t size);

	/**
	 * Создает строку из байтового массива @p ba.
	 */
	String(const Binary& ba);

	/**
	 * Создает строку из строки @p string.
	 */
	String(const std::string& string);

	/**
	 * Создает строку из строки @p string.
	 */
	String(const std::u16string& string);

	/**
	 * Создает копию строки @p other.
	 */
	String(const String& other);

	/**
	 * Создает строку, состоящую из одного символа @p ch.
	 */
	String(Char ch);

	/**
	 * Создает строку длиной в @p count символов и заполняет ее символом @p ch.
	 */
	String(size_t count, Char ch);

	/**
	 * Уничтожает строку.
	 */
	~String();

	/**
	 * Возвращает @c true, если строка является пустой строкой, созданной с помощью
	 * конструктора String().
	 */
	bool isNull() const;

	/**
	 * Возвращает @c true, если строка пустая.
	 */
	bool isEmpty() const;

	/**
	 * Возвращает длину строки.
	 */
	size_t length() const;

	/**
	 * Возвращает количество символов, которое можно добавить в начало строки без
	 * необходимости выполнения перераспределения памяти.
	 */
	size_t spaceAtBegin() const;

	/**
	 * Возвращает количество символов, которое можно добавить в конец строки без
	 * необходимости выполнения перераспределения памяти.
	 */
	size_t spaceAtEnd() const;

	/**
	 * Убеждается, что в начале буфера строки есть доступное пространство для как минимум
	 * @p size символов. В противном случае производит перераспределение памяти.
	 */
	void reserveAtBegin(size_t size);

	/**
	 * Убеждается, что в конце буфера строки есть доступное пространство для как минимум
	 * @p size символов. В противном случае производит перераспределение памяти.
	 */
	void reserveAtEnd(size_t size);

	/**
	 * Возвращает указатель на данные, содержащиеся в строке. Указатель может
	 * использоваться для чтения и записи данных.
	 */
	Char* data();

	/**
	 * Возвращает указатель на данные, содержащиеся в строке.
	 */
	const Char* data() const;

	/**
	 * Возвращает указатель на данные, содержащиеся в строке.
	 */
	const Char* constData() const;

	/**
	 * Возвращает @c true, если строка начинается с @p string, иначе возвращает @c false.
	 */
	bool startsWith(const String& string) const;

	/**
	 * Возвращает @c true, если первым символом строки является @p ch, иначе возвращает
	 * @c false.
	 */
	bool startsWith(Char ch) const;

	/**
	 * Возвращает @c true, если строка оканчивается на @p string, иначе возвращает
	 * @c false.
	 */
	bool endsWith(const String& string) const;

	/**
	 * Возвращает @c true, если последним символом строки является @p ch, иначе
	 * возвращает @c false.
	 */
	bool endsWith(Char ch) const;

	/**
	 * Производит поиск подстроки @p string начиная с позиции @p from и возвращает ее
	 * позицию. Если искомая подстрока не найдена, возвращается значение @c kNoPos.
	 */
	size_t indexOf(const String& string, size_t from = 0) const;

	/**
	 * Производит поиск символа @p ch начиная с позиции @p from и возвращает его позицию.
	 * Если искомый символ не найден, возвращается значение @c kNoPos.
	 */
	size_t indexOf(Char ch, size_t from = 0) const;

	/**
	 * Производит обратный поиск подстроки @p string начиная с позиции @p from и
	 * возвращает ее позицию. Если искомая подстрока не найдена, возвращается значение
	 * @c kNoPos. Если @p from равно @c kNoPos, то поиск будет осуществляться начиная с
	 * последнего символа строки.
	 */
	size_t lastIndexOf(const String& string, size_t from = kNoPos) const;

	/**
	 * Производит обратный поиск символа @p ch начиная с позиции @p from и возвращает его
	 * позицию. Если искомый символ не найден, возвращается значение @c kNoPos.Если
	 * @p from равно @c kNoPos, то поиск будет осуществляться начиная с последнего
	 * символа строки.
	 */
	size_t lastIndexOf(Char ch, size_t from = kNoPos) const;

	/**
	 * Очищает содержимое строки и делает ее пустой.
	 */
	void clear();

	/**
	 * Меняет местами содержимое строки с содержимым строки @p string.
	 */
	void swap(String& string);

	/**
	 * Присоединяет в конец строки строку @p string.
	 */
	String& append(const String& string);

	/**
	 * Присоединяет в конец строки строку @p string.
	 */
	String& append(const char* string);

	/**
	 *
	 */
	String& append(const Char* data, size_t count);

	/**
	 * Присоединяет в конец строки символ @p ch.
	 */
	String& append(Char ch);

	/**
	 * Присоединяет в начало строки строку @p string.
	 */
	String& prepend(const String& string);

	/**
	 * Присоединяет в начало строки строку @p string.
	 */
	String& prepend(const char* string);

	/**
	 * Присоединяет в начало строки символ @p ch.
	 */
	String& prepend(Char ch);

	/**
	 * Производит вставку @p string на позицию @p position. Если указанная позиция
	 * вставки больше или равна длине строки, то @p string добавляется в конец строки.
	 */
	String& insert(size_t position, const String& string);

	/**
	 * Производит вставку @p string на позицию @p position. Если указанная позиция
	 * вставки больше или равна длине строки, то @p string добавляется в конец строки.
	 */
	String& insert(size_t position, const char* string);

	/**
	 * Производит вставку @p ch на позицию @p position. Если указанная позиция вставки
	 * больше или равна длине строки, то @p ch добавляется в конец строки.
	 */
	String& insert(size_t position, Char ch);

	/**
	 * Удаляет @p count символов начиная с позиции @p position. Если указанная позиция
	 * @p position больше или равна длине строки, модификация не производится. Если
	 * @p position + @p count больше или равно длине строки, то строка обрезается по
	 * позиции @p position.
	 */
	String& remove(size_t position, size_t count);

	/**
	 * Удаляет каждое найденное входжение подстроки @p string.
	 */
	String& remove(const String& string);

	/**
	 * Удаляет каждое найденное входжение символа @p ch.
	 */
	String& remove(Char ch);

	/**
	 * Заменяет каждое найденное вхождение подстроки @p before на строку @p after.
	 */
	String& replace(const String& before, const String& after);

	/**
	 * Заменяет каждое найденное вхождение символа @p before на символ @p after.
	 */
	String& replace(Char before, Char after);

	/**
	 * Заменяет каждое найденное вхождение символа @p before на строку @p after.
	 */
	String& replace(Char before, const String& after);

	/**
	 * Заменяет каждое найденное вхождение подстроки @p before на символ @p after.
	 */
	String& replace(const String& before, Char after);

	/**
	 * Заменяет @p count символов начиная с позиции @p position на строку @p after.
	 */
	String& replace(size_t position, size_t count, const String& after);

	/**
	 * Заменяет @p count символов начиная с позиции @p position на символ @p after.
	 */
	String& replace(size_t position, size_t count, Char after);

	/**
	 * Устанавливает все символы строки в значение @p ch.
	 */
	String& fill(Char ch);

	/**
	 * Возвращает подстроку, которая содержит @p count начальных символов исходной
	 * строки. Если @p count больше длины строки, то возвращается исходная строка.
	 */
	String left(size_t count) const;

	/**
	 * Возвращает подстроку, которая содержит @p count символов исходной строки, начиная
	 * с позиции @p from. Если количество символов с позиции @p from до конца строки
	 * меньше, чем @p count, то возвращаются все символы исходной строки, начиная с
	 * позиции @p from.
	 */
	String middle(size_t from, size_t count = kNoPos) const;

	/**
	 * Возвращает подстроку, которая содержит @p count последних символов исходной
	 * строки. Если @p count больше длины строки, то возвращается исходная строка.
	 */
	String right(size_t count) const;

	/**
	 * Возвращает строку длиной @p width, окончание которой при необходимости
	 * заполненяется символом @p fill. Если @p truncate равно @c false и длина строки
	 * больше чем @p width, то будет возвращена копия исходной строки. Если @p truncate
	 * равно @c true и длина строки больше, чем @p width, то будет возвращено начало
	 * исходной строки длиной @p width.
	 */
	String leftJustified(size_t width, Char fill = ' ', bool truncate = false) const;

	/**
	 * Возвращает строку длиной @p width, начало и окончание которой при необходимости
	 * заполненяются символом @p fill. Если @p truncate равно @c false и длина строки
	 * больше чем @p width, то будет возвращена копия исходной строки. Если @p truncate
	 * равно @c true и длина строки больше, чем @p width, то будет возвращена середина
	 * исходной строки с дилной @p width.
	 */
	String middleJustified(size_t width, Char fill = ' ', bool truncate = false) const;

	/**
	 * Возвращает строку длиной @p width, начало которой при необходимости заполненяется
	 * символом @p fill. Если @p truncate равно @c false и длина строки больше чем
	 * @p width, то будет возвращена копия исходной строки. Если @p truncate равно
	 * @c true и длина строки больше, чем @p width, то будет возвращено окончание
	 * исходной строки длиной @p width.
	 */
	String rightJustified(size_t width, Char fill = ' ', bool truncate = false) const;

	/**
	 * Разделяет исходную строку на подстроки по разделителю @p sep. Если @p sep не
	 * содержится в исходной строке, то возвращаемый результат будет содержать
	 * единственный элемент, в котором будет находиться копия исходной строки. Если
	 * @p behaviour равен @c String::kSkipEmptyParts, то пустые строки не будут
	 * включаться в результат.
	 */
	StringList split(Char sep, SplitBahavior behavior = kKeepEmptyParts) const;

	/**
	 * Разделяет исходную строку на подстроки по разделителю @p sep. Если @p sep не
	 * содержится в исходной строке, то возвращаемый результат будет содержать
	 * единственный элемент, в котором будет находиться копия исходной
	 * строки. Если @p behaviour равен @c String::kSkipEmptyParts, то пустые строки не
	 * будут включаться в результат.
	 */
	StringList split(const String& sep, SplitBahavior behavior = kKeepEmptyParts) const;

	/**
	 * Удаляет @p n последних символов из строки. Если @p n больше или равно длине
	 * строки, она становится пустой.
	 */
	void chop(size_t n);

	/**
	 * Обрезает строку по указанной позиции @p position. Если @p position больше или
	 * равна длине строки, то изменений не производится.
	 */
	void truncate(size_t position);

	/**
	 * Возвращает копию исходной строки, из которой удалены все пробельные символы в
	 * начале и в конце, а также все последовательности остальных пробельных символов
	 * заменены на единственный символ пробела.
	 */
	String simplified() const;

	/**
	 * Возвращает копию исходной строки, из которой удалены все пробельные символы в
	 * начале и в конце.
	 */
	String trimmed() const;

	/**
	 * Преобразует строку из внутреннего представления в UTF-8 и возвращает результат в
	 * байтовом массиве.
	 */
	Binary toUtf8() const;

	/**
	 * Возвращает строку, инициализарованную первыми @p size символами C-строки в
	 * кодировке UTF-8.
	 */
	static String fromUtf8(const char* string, size_t size = kNoPos);

	/**
	 * Возвращает строку, инициализарованную первыми @p size символами массива
	 * @p unicode.
	 */
	static String fromRawData(const Char* unicode, size_t size = kNoPos);

	String& operator=(const String& other);
	String& operator=(Char ch);

	bool operator==(const String& string) const;
	bool operator!=(const String& string) const;
	bool operator<(const String& string) const;
	bool operator<=(const String& string) const;
	bool operator>(const String& string) const;
	bool operator>=(const String& string) const;

	String& operator+=(const String& string);
	String& operator+=(Char ch);

	Char operator[](size_t position) const;
	CharRef operator[](size_t position);

private:
	struct Buffer;

	Buffer* buffer_;
	ch16* begin_;
	ch16* end_;

	static std::unique_ptr<Buffer> sharedNull_;

	static Buffer* sharedNull();

	String(const String& other, size_t position, size_t count);

	size_t hashingSearch(const ch16* data, size_t size, size_t from) const;
	size_t hashingReverseSearch(const ch16* data, size_t size, size_t from) const;

	size_t boyerMooreSearch(const ch16* data, size_t size, size_t from) const;
	size_t boyerMooreReverseSearch(const ch16* data, size_t size, size_t from) const;

	void makeUnique();

	size_t indexOf(const ch16* data, size_t size, size_t from = 0) const;
	size_t lastIndexOf(const ch16* data, size_t size, size_t from = 0) const;

	String& append(const ch16* data, size_t size);
	String& prepend(const ch16* data, size_t size);
	String& insert(size_t position, const ch16* data, size_t size);
	String& remove(const ch16* data, size_t size, int dummy);

	String& replace(size_t position, size_t count, const ch16* after, size_t afterSize);

	String& replace(const ch16* before, size_t beforeSize, const ch16* after,
			size_t afterSize);
};


String operator+(const String& string1, const String& string2);
String operator+(Char ch, const String& string);
String operator+(const String& string, Char ch);


class CharRef {
public:
	operator Char() const
	{
		if(pos_ < string_.length())
			return string_.constData()[pos_];

		return Char();
	}

	bool isDigit() const
	{
		return string_.data()[pos_].isDigit();
	}

	int digitValue() const
	{
		return string_.data()[pos_].digitValue();
	}

	CharRef& operator=(const char ch)
	{
		string_.data()[pos_] = ch;
		return *this;
	}

	CharRef& operator=(const CharRef& ref)
	{
		string_.data()[pos_] = ref.string_.constData()[ref.pos_];
		return *this;
	}

	bool operator==(Char ch) const
	{
		return string_.constData()[pos_] == ch;
	}

	bool operator!=(Char ch) const
	{
		return string_.constData()[pos_] != ch;
	}

	bool operator<(Char ch) const
	{
		return string_.constData()[pos_] < ch;
	}

	bool operator>(Char ch) const
	{
		return string_.constData()[pos_] > ch;
	}

	bool operator<=(Char ch) const
	{
		return string_.constData()[pos_] <= ch;
	}

	bool operator>=(Char ch) const
	{
		return string_.constData()[pos_] >= ch;
	}

private:
	friend class String;

	String& string_;
	size_t pos_;


	CharRef(String& string, size_t pos) :
		string_(string),
		pos_(pos)
	{
	}
};


inline bool operator==(Char ch, const CharRef& ref)
{
	return ref == ch;
}


inline bool operator!=(Char ch, const CharRef& ref)
{
	return ref != ch;
}


inline bool operator<(Char ch, const CharRef& ref)
{
	return ref > ch;
}


inline bool operator>(Char ch, const CharRef& ref)
{
	return ref < ch;
}


inline bool operator<=(Char ch, const CharRef& ref)
{
	return ref >= ch;
}


inline bool operator>=(Char ch, const CharRef& ref)
{
	return ref <= ch;
}


} // namespace Tech


#endif // TECH_STRING_H
