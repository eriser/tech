#ifndef TECH_FORMAT_H
#define TECH_FORMAT_H

#include <tech/string.h>
#include <tech/utils.h>


namespace Tech {


class Formatter {
public:
	struct Flags {
		enum Alignment {
			kAlignLeft,     ///< Выравнивание по левому краю доступного пространства
			kAlignRight,    ///< Выравнивание по правому краю доступного пространства
			kAlignCenter,   ///< Выравнивание по центру доступного пространства
			kAlignSignAware ///< Выравнивание по правому краю с заполнением после знака
		};

		enum Type {
			kDefault,       ///< Использовать представление по умолчанию
			kCharacter,     ///< Символ
			kBinary,        ///< Целое число в двоичной форме
			kOctal,         ///< Целое число в восьмиричной форме
			kDecimal,       ///< Целое число в десятичной форме
			kHexadecimal,   ///< Целое число в шестнадцатеричной форме
			kFixedPoint,    ///< Вещественное число с фиксированной точкой
			kScientific,    ///< Вещественное число в научном формате
			kGeneral,       ///< Вещественное число в общем формате
			kString,        ///< Строка символов
			kPercentage     ///< Значение умножается на 100 и приводится к kFixedPoint %
		};

		Char fill;
		Alignment alignment;
		Char positiveSign;
		bool showBasePrefix;
		uint precision;
		uint minimumWidth;
		Type type;
		bool useUpperCase;

		Flags() :
			fill(' '),
			alignment(kAlignLeft),
			positiveSign(0),
			showBasePrefix(false),
			precision(6),
			minimumWidth(0),
			type(kDefault),
			useUpperCase(false)
		{
		}
	};

	/**
	 * Возвращает строку, собранную путем подстановки в шаблон @p string значений
	 * @p args, преобразованных в строковую форму в соответствии с указанными в шаблоне
	 * спецификаторами формата.
	 *
	 * Подстановочные поля шаблона
	 * placeholder ::=  '{' [index] [':' format_spec] '}'
	 * index       ::=  integer
	 *
	 * Спецификатор формата для примитивных и строковых типов (format_spec):
	 * format_spec ::=  [[fill]align][sign][#][0][width][.precision][type]
	 * fill        ::=  character
	 * align       ::=  '<' | '>' | '=' | '^'
	 * sign        ::=  '+' | '-' | ' '
	 * width       ::=  integer
	 * precision   ::=  integer
	 * type        ::=  'b' | 'c' | 'd' | 'e' | 'E' | 'f' | 'F' | 'g' | 'G' | 'o' | 's' |
	 *                  'x' | 'X' | '%'
	 *
	 * Опции выравнивания (align):
	 * '<' выравнивает значение в доступном пространстве по левому краю (по умолчнию)
	 * '>' выравнивает значение в доступном пространстве по правому краю
	 * '^' выравнивает значение в доступном пространстве по центру
	 * '='
	 *
	 *
	 */
	template<typename ...Args>
	static String format(const String& string, Args&&... args);

	static Flags parseDefaultSpec(const String& spec);
	static String alignString(const String& string, const Flags& flags);
	static String unsignedToString(u64 value, int base, const Flags& flags);
	static String floatToString(float value, const Flags& flags);
	static String doubleToString(double value, const Flags& flags);

private:
	class Renderer {
	public:
		virtual String render(const String& spec) = 0;
	};

	template<typename T>
	class ValueRenderer final : public Renderer {
	public:
		ValueRenderer(T value);
		String render(const String& spec) override;

		T value_;
	};

	template<int N>
	class Impl {
	public:
		Impl(const String& string);

		String format(int index);

		template<typename First, typename ...Rest>
		String format(int index, First&& first, Rest&&... rest);

	private:
		String string_;
		Box<Renderer> renderers_[N];
		size_t from_;
		uint nextIndex_;

		const Char* parseInteger(const Char* begin, const Char* end, uint* value) const;
	};
};


String formatValue(bool value, const String& spec = String());
String formatValue(int value, const String& spec = String());
String formatValue(float value, const String& spec = String());
String formatValue(double value, const String& spec = String());


template<typename T, EnableIf<
		IsInteger<T>,
		Not<std::is_same<T, bool>>,
		Not<std::is_same<T, char>>,
		Not<std::is_same<T, wchar_t>>,
		Not<std::is_same<T, ch16>>,
		Not<std::is_same<T, ch32>>>...>
String formatValue(T value, const String& spec)
{
	// FIXME
	return "";
}

String formatValue(const String& value, const String& spec = String());
String formatValue(const ByteArray& value, const String& spec = String());
String formatValue(const char* value, const String& spec = String());


//
// Formatter
//
template<typename ...Args>
String Formatter::format(const String& string, Args&&... args)
{
	Formatter::Impl<sizeof...(args)> formatter(string);
	return formatter.format(0, std::forward<Args>(args)...);
}


//
// ValueRenderer
//
template<typename T>
Formatter::ValueRenderer<T>::ValueRenderer(T value) :
	value_(std::forward<T>(value))
{
}


template<typename T>
String Formatter::ValueRenderer<T>::render(const String& spec)
{
	return Tech::formatValue(value_, spec);
}


//
// Impl
//
template<int N>
Formatter::Impl<N>::Impl(const String& string) :
	string_(string),
	from_(0),
	nextIndex_(0)
{
}


template<int N>
String Formatter::Impl<N>::format(int index)
{
	UNUSED(index);

	// Сумма длин значений всех аргументов после преобразования в строковую форму заранее
	// неизвестна, поэтому здесь резервируется приблизительная минимальная величина
	String result;
	result.reserveAtEnd(string_.length() + N);

	const Char* begin = string_.constData();
	const Char* end = begin + string_.length();
	const Char* pos = begin;
	const Char* specBegin;
	const Char* specEnd;

	while(pos < end) {
		pos = std::find(pos, end, u'{');
		if(pos == end)
			break;

		++pos;
		if(pos < end && *pos == u'{') {
			++pos;
			continue;
		}

		size_t length = pos - begin - 1;
		uint index;

		if(pos->isDigit()) {
			pos = parseInteger(pos, end, &index);
			if(pos == end)
				break;
		}
		else {
			index = nextIndex_;
		}

		if(*pos == u':') {
			specBegin = ++pos;
		}
		else {
			specBegin = nullptr;
		}

		pos = std::find(pos, end, u'}');
		if(pos == end)
			break;

		if(index >= N) {
			++pos;
			continue;
		}

		if(specBegin) {
			specEnd = pos;
		}
		else {
			specEnd   = nullptr;
		}

		result.append(begin, length);
//		result.append(renderers_[index]->render(specBegin, specEnd));

		size_t from = specBegin - string_.constData();
		size_t count = specEnd - specBegin;
		result.append(renderers_[index]->render(string_.middle(from, count)));
		begin = ++pos;
		nextIndex_ = index + 1;
	}

	result.append(begin, end - begin);
	return result;
}


template<int N>
template<typename First, typename ...Rest>
String Formatter::Impl<N>::format(int index, First&& first, Rest&&... rest)
{
	renderers_[index].reset(new ValueRenderer<First&&>(std::forward<First>(first)));
	return format(index + 1, std::forward<Rest>(rest)...);
}


template<int N>
const Char* Formatter::Impl<N>::parseInteger(const Char* begin, const Char* end,
		uint* value) const
{
	*value = 0;

	while(begin < end) {
		int digit = begin->digitValue();
		if(digit == -1)
			break;

		*value = *value * 10 + digit;
		++begin;
	}

	return begin;
}


} // namespace Tech


#endif // TECH_FORMAT_H
