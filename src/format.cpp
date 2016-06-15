#include <tech/format.h>

#include <cmath>


namespace Tech {


Formatter::Flags Formatter::parseDefaultSpec(const String& spec)
{
	Flags flags;

	if(spec.isEmpty())
		return flags;

	const Char* pos = spec.constData();
	const Char* end = pos + spec.length();

	auto parseAlignment = [&flags](const Char* p) -> bool {
		if(*p == '<') {
			flags.alignment = Flags::kAlignLeft;
		}
		else if(*p == '>') {
			flags.alignment = Flags::kAlignRight;
		}
		else if(*p == '^') {
			flags.alignment = Flags::kAlignCenter;
		}
		else if(*p == '=') {
			flags.alignment = Flags::kAlignSignAware;
		}
		else {
			return false;
		}

		return true;
	};

	auto parseInteger = [& pos, & end](uint* value) -> bool {
		const Char* temp = pos;

		while(pos < end) {
			int digit = pos->digitValue();
			if(digit == -1)
				break;

			*value = *value * 10 + digit;
			++pos;
		}

		return pos != temp;
	};

	if(spec.length() > 1 && parseAlignment(pos + 1)) {
		flags.fill = *pos;
		pos += 2;
	}
	else if(parseAlignment(pos)) {
		++pos;
	}

	if(pos == end)
		return flags;

	if(*pos == '+') {
		flags.positiveSign = '+';
		++pos;
	}
	else if(*pos == '-') {
		++pos;
	}
	else if(*pos == ' ') {
		flags.positiveSign = ' ';
		++pos;
	}

	if(pos == end)
		return flags;

	if(*pos == '#') {
		flags.showBasePrefix = true;
		if(++pos == end)
			return flags;
	}

	if(*pos == '0') {
		flags.fill = '0';
		flags.alignment = Flags::kAlignSignAware;
		if(++pos == end)
			return flags;
	}

	if(parseInteger(&flags.minimumWidth)) {
		if(pos == end)
			return flags;
	}

	if(*pos == '.') {
		++pos;

		if(parseInteger(&flags.precision)) {
			if(pos == end)
				return flags;
		}
	}

	switch(pos->unicode()) {
	case u'b':
		flags.type = Flags::kBinary;
		break;

	case u'c':
		flags.type = Flags::kCharacter;
		break;

	case u'd':
		flags.type = Flags::kDecimal;
		break;

	case u'e':
		flags.type = Flags::kScientific;
		break;

	case u'E':
		flags.type = Flags::kScientific;
		flags.useUpperCase = true;
		break;

	case u'f':
		flags.type = Flags::kFixedPoint;
		break;

	case u'F':
		flags.type = Flags::kFixedPoint;
		flags.useUpperCase = true;
		break;

	case u'g':
		break;

	case u'G':
		break;

	case u'o':
		flags.type = Flags::kOctal;
		break;

	case u's':
		break;

	case u'x':
		flags.type = Flags::kHexadecimal;
		break;

	case u'X':
		flags.type = Flags::kHexadecimal;
		flags.useUpperCase = true;
		break;

	case u'%':
		flags.type = Flags::kPercentage;
		break;
	}

	return flags;
}


String Formatter::alignString(const String& string, const Flags& flags)
{
	int fillCount = flags.minimumWidth - string.length();

	if(fillCount > 0) {
		String result(flags.minimumWidth, flags.fill);
		size_t pos = 0;

		if(flags.alignment == Flags::kAlignRight ||
		   flags.alignment == Flags::kAlignSignAware) {
			pos = fillCount;
		}
		else if(flags.alignment == Flags::kAlignCenter) {
			pos = fillCount / 2;
		}

		return result.replace(pos, string.length(), string);
	}

	return string;
}


String formatValue(bool value, const String& spec)
{
	Formatter::Flags flags = Formatter::parseDefaultSpec(spec);

	if(flags.type == Formatter::Flags::kDefault) {
		if(value)
			return Formatter::alignString(u"true", flags);

		return Formatter::alignString(u"false", flags);
	}

	// FIXME spec -> flags
	return formatValue(static_cast<int>(value), spec);
}


String formatValue(i8 value, const String& spec)
{
	return formatValue(static_cast<i64>(value), spec);
}


String formatValue(u8 value, const String& spec)
{
	return formatValue(static_cast<u64>(value), spec);
}


String formatValue(i16 value, const String& spec)
{
	return formatValue(static_cast<i64>(value), spec);
}


String formatValue(u16 value, const String& spec)
{
	return formatValue(static_cast<u64>(value), spec);
}


String formatValue(i32 value, const String& spec)
{
	return formatValue(static_cast<i64>(value), spec);

}


String formatValue(u32 value, const String& spec)
{
	return formatValue(static_cast<u64>(value), spec);
}


String formatValue(i64 value, const String& spec)
{
	Formatter::Flags flags = Formatter::parseDefaultSpec(spec);

	int base;
	ch16 basePrefix[3];
	basePrefix[0] = '0';
	basePrefix[2] = '\0';

	switch(flags.type) {
	case Formatter::Flags::kBinary:
		base = 2;
		basePrefix[1] = 'b';
		break;

	case Formatter::Flags::kOctal:
		base = 8;
		basePrefix[1] = 'o';
		break;

	case Formatter::Flags::kHexadecimal:
		base = 16;
		basePrefix[1] = 'x';
		break;

	default:
		base = 10;
		basePrefix[1] = 'd';
		break;
	}

	String prefix;

	if(value >= 0) {
		if(flags.positiveSign != 0)
			prefix += flags.positiveSign;
	}
	else {
		prefix += '-';
	}

	if(flags.showBasePrefix)
		prefix += basePrefix;

	String string = Formatter::unsignedToString(std::abs(value), base, flags);

	size_t totalLength = string.length() + prefix.length();
	int fillCount = flags.minimumWidth - totalLength;
	if(fillCount <= 0)
		return prefix + string;

	String result(flags.minimumWidth, flags.fill);

	if(flags.alignment == Formatter::Flags::kAlignLeft) {
		result.replace(0, prefix.length(), prefix);
		result.replace(prefix.length(), string.length(), string);
	}
	else if(flags.alignment == Formatter::Flags::kAlignRight) {
		result.replace(fillCount, prefix.length(), prefix);
		result.replace(fillCount + prefix.length(), string.length(), string);
	}
	else if(flags.alignment == Formatter::Flags::kAlignCenter) {
		size_t count = fillCount / 2;
		result.replace(count, prefix.length(), prefix);
		result.replace(count + prefix.length(), string.length(), string);
	}
	else if(flags.alignment == Formatter::Flags::kAlignSignAware) {
		result.replace(0, prefix.length(), prefix);
		result.replace(fillCount + prefix.length(), string.length(), string);
	}

	return result;
}


String formatValue(u64 value, const String& spec)
{
	Formatter::Flags flags = Formatter::parseDefaultSpec(spec);

	int base;
	ch16 basePrefix[3];
	basePrefix[0] = '0';
	basePrefix[2] = '\0';

	switch(flags.type) {
	case Formatter::Flags::kBinary:
		base = 2;
		basePrefix[1] = 'b';
		break;

	case Formatter::Flags::kOctal:
		base = 8;
		basePrefix[1] = 'o';
		break;

	case Formatter::Flags::kHexadecimal:
		base = 16;
		basePrefix[1] = 'x';
		break;

	default:
		base = 10;
		basePrefix[1] = 'd';
		break;
	}

	String prefix;

	if(flags.positiveSign != 0)
		prefix += flags.positiveSign;

	if(flags.showBasePrefix)
		prefix += basePrefix;

	String string = Formatter::unsignedToString(value, base, flags);

	size_t totalLength = string.length() + prefix.length();
	int fillCount = flags.minimumWidth - totalLength;
	if(fillCount <= 0)
		return prefix + string;

	String result(flags.minimumWidth, flags.fill);

	if(flags.alignment == Formatter::Flags::kAlignLeft) {
		result.replace(0, prefix.length(), prefix);
		result.replace(prefix.length(), string.length(), string);
	}
	else if(flags.alignment == Formatter::Flags::kAlignRight) {
		result.replace(fillCount, prefix.length(), prefix);
		result.replace(fillCount + prefix.length(), string.length(), string);
	}
	else if(flags.alignment == Formatter::Flags::kAlignCenter) {
		size_t count = fillCount / 2;
		result.replace(count, prefix.length(), prefix);
		result.replace(count + prefix.length(), string.length(), string);
	}
	else if(flags.alignment == Formatter::Flags::kAlignSignAware) {
		result.replace(0, prefix.length(), prefix);
		result.replace(fillCount + prefix.length(), string.length(), string);
	}

	return result;
}


String Formatter::unsignedToString(u64 value, int base, const Formatter::Flags& flags)
{
	String result;
	ch16 zero = '0';
	ch16 ten = flags.useUpperCase ? 'A' : 'a';

	do {
		ch16 ch;
		int digit = value % base;

		if(digit < 10) {
			ch = zero + digit;
		}
		else {
			ch = ten + digit - 10;
		}

		result.prepend(ch);
		value /= base;
	} while(value);

	return result;
}


String Formatter::floatToString(float value, const Formatter::Flags& flags)
{
	// FIXME


	return String();
}


String Formatter::doubleToString(double value, const Formatter::Flags& flags)
{
	// FIXME
	return String();
}


String formatValue(float value, const String& spec)
{
	Formatter::Flags flags = Formatter::parseDefaultSpec(spec);

	switch(flags.type) {
	case Formatter::Flags::kPercentage:
		break;

	case Formatter::Flags::kFixedPoint:
		break;

	case Formatter::Flags::kScientific:
		break;

	case Formatter::Flags::kGeneral:
	default:
		break;
	}

	String prefix;

	if(value >= 0) {
		if(flags.positiveSign != 0)
			prefix += flags.positiveSign;
	}
	else {
		prefix += '-';
	}

	String string = Formatter::floatToString(std::abs(value), flags);
	size_t totalLength = string.length() + prefix.length();
	int fillCount = flags.minimumWidth - totalLength;
	if(fillCount <= 0)
		return prefix + string;

	String result(flags.minimumWidth, flags.fill);

	if(flags.alignment == Formatter::Flags::kAlignLeft) {
		result.replace(0, prefix.length(), prefix);
		result.replace(prefix.length(), string.length(), string);
	}
	else if(flags.alignment == Formatter::Flags::kAlignRight) {
		result.replace(fillCount, prefix.length(), prefix);
		result.replace(fillCount + prefix.length(), string.length(), string);
	}
	else if(flags.alignment == Formatter::Flags::kAlignCenter) {
		size_t count = fillCount / 2;
		result.replace(count, prefix.length(), prefix);
		result.replace(count + prefix.length(), string.length(), string);
	}
	else if(flags.alignment == Formatter::Flags::kAlignSignAware) {
		result.replace(0, prefix.length(), prefix);
		result.replace(fillCount + prefix.length(), string.length(), string);
	}

	return result;
}


String formatValue(double value, const String& spec)
{
	Formatter::Flags flags = Formatter::parseDefaultSpec(spec);

	switch(flags.type) {
	case Formatter::Flags::kPercentage:
		break;

	case Formatter::Flags::kFixedPoint:
		break;

	case Formatter::Flags::kScientific:
		break;

	case Formatter::Flags::kGeneral:
	default:
		break;
	}

	String prefix;

	if(value >= 0) {
		if(flags.positiveSign != 0)
			prefix += flags.positiveSign;
	}
	else {
		prefix += '-';
	}

	String string = Formatter::doubleToString(std::abs(value), flags);
	size_t totalLength = string.length() + prefix.length();
	int fillCount = flags.minimumWidth - totalLength;
	if(fillCount <= 0)
		return prefix + string;

	String result(flags.minimumWidth, flags.fill);

	if(flags.alignment == Formatter::Flags::kAlignLeft) {
		result.replace(0, prefix.length(), prefix);
		result.replace(prefix.length(), string.length(), string);
	}
	else if(flags.alignment == Formatter::Flags::kAlignRight) {
		result.replace(fillCount, prefix.length(), prefix);
		result.replace(fillCount + prefix.length(), string.length(), string);
	}
	else if(flags.alignment == Formatter::Flags::kAlignCenter) {
		size_t count = fillCount / 2;
		result.replace(count, prefix.length(), prefix);
		result.replace(count + prefix.length(), string.length(), string);
	}
	else if(flags.alignment == Formatter::Flags::kAlignSignAware) {
		result.replace(0, prefix.length(), prefix);
		result.replace(fillCount + prefix.length(), string.length(), string);
	}

	return result;
}


String formatValue(const String& value, const String& spec)
{
	Formatter::Flags flags = Formatter::parseDefaultSpec(spec);

	if(value.length() < flags.minimumWidth) {
		String result(flags.minimumWidth, flags.fill);
		size_t pos = 0;

		if(flags.alignment == Formatter::Flags::kAlignRight) {
			pos = flags.minimumWidth - value.length();
		}
		else if(flags.alignment == Formatter::Flags::kAlignCenter) {
			pos = (flags.minimumWidth - value.length()) / 2;
		}

		return result.replace(pos, value.length(), value);
	}
/*	else if(flags.maximumWidth && value.length() > flags.maximumWidth) {
		size_t pos = 0;

		if(flags.alignment == FormatFlags::kRight) {
			pos = value.length() - flags.maximumWidth;
		}
		else if(flags.alignment == FormatFlags::kCenter ||
				flags.alignment == FormatFlags::kJustify) {
			pos = (value.length() - flags.maximumWidth) / 2;
		}

		return value.middle(pos, flags.maximumWidth);
	}*/

	return value;
}


String formatValue(const Binary& value, const String& spec)
{
	Formatter::Flags flags = Formatter::parseDefaultSpec(spec);

	if(value.length() < flags.minimumWidth) {
		String result(flags.minimumWidth, flags.fill);
		size_t pos = 0;

		if(flags.alignment == Formatter::Flags::kAlignRight) {
			pos = flags.minimumWidth - value.length();
		}
		else if(flags.alignment == Formatter::Flags::kAlignCenter) {
			pos = (flags.minimumWidth - value.length()) / 2;
		}

		return result.replace(pos, value.length(), value);
	}
/*	else if(flags.maximumWidth && value.length() > flags.maximumWidth) {
		size_t pos = 0;

		if(flags.alignment == FormatFlags::kRight) {
			pos = value.length() - flags.maximumWidth;
		}
		else if(flags.alignment == FormatFlags::kCenter ||
				flags.alignment == FormatFlags::kJustify) {
			pos = (value.length() - flags.maximumWidth) / 2;
		}

		return value.middle(pos, flags.maximumWidth);
	}*/

	return value;
}


String formatValue(const char* value, const String& spec)
{
	return formatValue(String(value), spec);
}


} // namespace Tech
