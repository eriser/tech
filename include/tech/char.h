#ifndef TECH_CHAR_H
#define TECH_CHAR_H

#include <tech/types.h>


namespace Tech {


class Char final {
public:
	Char();
	Char(ch16 ch);

	ch16 unicode() const;

	bool isSpace() const;
	bool isDigit() const;
	int digitValue() const;

	bool operator==(Char ch) const;
	bool operator!=(Char ch) const;
	bool operator<(Char ch) const;
	bool operator>(Char ch) const;
	bool operator<=(Char ch) const;
	bool operator>=(Char ch) const;

private:
	ch16 value_;
};


inline
Char::Char() :
	value_(0)
{
}


inline
Char::Char(ch16 ch) :
	value_(ch)
{
}


inline
ch16 Char::unicode() const
{
	return value_;
}


inline
bool Char::isSpace() const
{
	// FIXME
	return false;
}


inline
bool Char::isDigit() const
{
	return (value_ >= u'0' && value_ <= u'9');
}


inline
int Char::digitValue() const
{
	if(!isDigit())
		return -1;

	return value_ - u'0';
}


inline
bool Char::operator==(Char ch) const
{
	return value_ == ch.value_;
}


inline
bool Char::operator!=(Char ch) const
{
	return value_ != ch.value_;
}


inline
bool Char::operator<(Char ch) const
{
	return value_ < ch.value_;
}


inline
bool Char::operator>(Char ch) const
{
	return value_ > ch.value_;
}


inline
bool Char::operator<=(Char ch) const
{
	return value_ <= ch.value_;
}


inline
bool Char::operator>=(Char ch) const
{
	return value_ >= ch.value_;
}


} // namespace Tech


#endif // TECH_CHAR_H
