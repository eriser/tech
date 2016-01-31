#ifndef TECH_DURATION_H
#define TECH_DURATION_H

#include <tech/types.h>


namespace Tech {


class Duration {
public:
	static const i64 kMsecsPerSecond   = 1000;
	static const i64 kSecondsPerMinute = 60;
	static const i64 kMinutesPerHour   = 60;
	static const i64 kHoursPerDay      = 24;
	static const i64 kDaysPerWeek      = 7;
	static const i64 kMonthsPerYear    = 12;
	static const i64 kDaysPerYear      = 365;
	static const i64 kDaysPerLeapYear  = 366;

	static const i64 kMsecsPerMinute   = kMsecsPerSecond * kSecondsPerMinute;
	static const i64 kMsecsPerHour     = kMsecsPerMinute * kMinutesPerHour;
	static const i64 kMsecsPerDay      = kMsecsPerHour   * kHoursPerDay;
	static const i64 kMsecsPerWeek     = kMsecsPerDay    * kDaysPerWeek;
	static const i64 kMsecsPerYear     = kMsecsPerDay    * kDaysPerYear;
	static const i64 kMsecsPerLeapYear = kMsecsPerDay    * kDaysPerLeapYear;

	explicit constexpr Duration(i64 mseconds = 0);
	Duration(int hours, int minutes, int seconds, int mseconds = 0);

	Duration(int weeks, int days, int hours, int minutes, int seconds, int mseconds = 0);

	void setDuration(int hours, int minutes, int seconds, int mseconds = 0);

	void setDuration(int weeks, int days, int hours, int minutes, int seconds,
			int mseconds = 0);

	bool isNull() const;
	bool isNegative() const;

	int weeks() const;
	int days() const;
	int hours() const;
	i64 minutes() const;
	i64 seconds() const;
	i64 mseconds() const;

	Duration& operator+=(const Duration& other);
	Duration& operator-=(const Duration& other);
	Duration& operator*=(int value);
	Duration& operator*=(float value);
	Duration& operator/=(int value);
	Duration& operator/=(float value);
	Duration& operator%=(int value);
	const Duration operator+(const Duration& other) const;
	const Duration operator-(const Duration& other) const;
	const Duration operator*(int value) const;
	const Duration operator*(float value) const;
	const Duration operator/(int value) const;
	const Duration operator/(float value) const;
	const Duration operator%(int value) const;
	bool operator==(const Duration& other) const;
	bool operator!=(const Duration& other) const;
	bool operator<(const Duration& other) const;
	bool operator>(const Duration& other) const;
	bool operator<=(const Duration& other) const;
	bool operator>=(const Duration& other) const;
	bool operator!() const;
	Duration operator-();

	// Duration from begin of the Epoch (1970) - the current time
	static Duration fromEpoch();

	static Duration second();
	static Duration minute();
	static Duration hour();
	static Duration day();
	static Duration week();
	static Duration year();
	static Duration leapYear();

	static Duration seconds(i64 count);
	static Duration minutes(i64 count);
	static Duration hours(i64 count);
	static Duration days(i64 count);

private:
	i64 msecs_;
};


inline
constexpr Duration::Duration(i64 mseconds) :
	msecs_(mseconds)
{
}


const Duration operator*(int value, const Duration& other);
const Duration operator*(float value, const Duration& other);


namespace Literals {


inline
constexpr Duration operator""_h(ulonglong hours)
{
	return Duration(Duration::kMsecsPerHour * hours);
}


inline
constexpr Duration operator""_h(long double hours)
{
	return Duration(Duration::kMsecsPerHour * hours);
}


inline
constexpr Duration operator""_m(ulonglong minutes)
{
	return Duration(Duration::kMsecsPerMinute * minutes);
}


inline
constexpr Duration operator""_m(long double minutes)
{
	return Duration(Duration::kMsecsPerMinute * minutes);
}


inline
constexpr Duration operator""_s(ulonglong seconds)
{
	return Duration(Duration::kMsecsPerSecond * seconds);
}


inline
constexpr Duration operator""_s(long double seconds)
{
	return Duration(Duration::kMsecsPerSecond * seconds);
}


inline
constexpr Duration operator""_ms(ulonglong msecs)
{
	return Duration(msecs);
}


inline
constexpr Duration operator""_ms(long double msecs)
{
	return Duration(msecs);
}


} // namespace Literals


} // namespace Tech


#endif // TECH_DURATION_H
