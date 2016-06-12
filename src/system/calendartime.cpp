#include <tech/system/calendartime.h>

#include <algorithm>
#include <numeric>
#include <tech/utils.h>


namespace Tech {


static const i64 kMsecsPer4Years = Duration::kMsecsPerYear * 3 +
		Duration::kMsecsPerLeapYear;

static const i64 kMsecsPer100Years = kMsecsPer4Years * 25 -	Duration::kMsecsPerDay;

static const i64 kMsecsPer400Years = kMsecsPer100Years * 4 + Duration::kMsecsPerDay;

static const i64 kEpochMsecs = kMsecsPer400Years * 4 + kMsecsPer100Years * 3
		+ kMsecsPer4Years * 17 + Duration::kMsecsPerYear * 2;

static const int kDaysPerMonth[Duration::kMonthsPerYear] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const int kDaysPerMonthLeap[Duration::kMonthsPerYear] = {
	31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const int kDaysBeforeMonth[Duration::kMonthsPerYear] = {
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};

static const int kDaysBeforeMonthLeap[Duration::kMonthsPerYear] = {
	0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335
};


CalendarTime::CalendarTime(int monthDay, int month, int year, int hour,
		int minute,	int second, int msecond)
{
	setDate(monthDay, month, year);
	setTime(hour, minute, second, msecond);
}


CalendarTime::CalendarTime(const Duration& duration)
{
	i64 msecs = kEpochMsecs + duration.mseconds();

	int y400s = msecs / kMsecsPer400Years;
	msecs %= kMsecsPer400Years;

	int y100s = msecs / kMsecsPer100Years;
	msecs %= kMsecsPer100Years;

	int y4s = msecs / kMsecsPer4Years;
	msecs %= kMsecsPer4Years;

	int y1s = msecs / Duration::kMsecsPerYear;
	msecs %= Duration::kMsecsPerYear;

	year_ = y400s * 400 + y100s * 100 + y4s * 4 + y1s;
	yearDay_ = msecs / Duration::kMsecsPerDay + 1;
	msecs %= Duration::kMsecsPerDay;

	const int* offsetTable;
	const int* pos;

	if(isLeapYear()) {
		offsetTable = kDaysBeforeMonthLeap;
	}
	else {
		offsetTable = kDaysBeforeMonth;
	}

	pos = std::lower_bound(offsetTable, offsetTable + Duration::kMonthsPerYear, yearDay_);
	month_ = pos - offsetTable;
	monthDay_ = yearDay_ - offsetTable[month_-1];
	weekDay_ = weekDayOf(monthDay_, month_, year_);

	hour_ = msecs / Duration::kMsecsPerHour;
	msecs %= Duration::kMsecsPerHour;

	minute_ = msecs / Duration::kMsecsPerMinute;
	msecs %= Duration::kMsecsPerMinute;

	second_ = msecs / Duration::kMsecsPerSecond;
	msecond_ = msecs % Duration::kMsecsPerSecond;
}


bool CalendarTime::operator==(const CalendarTime& other) const
{
	return year_ == other.year_ && yearDay_ == other.yearDay_ && hour_ == other.hour_ &&
			minute_ == other.minute_ && second_ == other.second_ &&
			msecond_ == other.msecond_;
}


bool CalendarTime::operator!=(const CalendarTime& other) const
{
	return !(*this == other);
}


bool CalendarTime::operator<(const CalendarTime& other) const
{
	return !(*this >= other);
}


bool CalendarTime::operator>(const CalendarTime& other) const
{
	return !(*this <= other);
}


bool CalendarTime::operator<=(const CalendarTime& other) const
{
	return year_ <= other.year_ && yearDay_ <= other.yearDay_ && hour_ <= other.hour_ &&
			minute_ <= other.minute_ && second_ <= other.second_ &&
			msecond_ <= other.msecond_;
}


bool CalendarTime::operator>=(const CalendarTime& other) const
{
	return year_ >= other.year_ && yearDay_ >= other.yearDay_ && hour_ >= other.hour_ &&
			minute_ >= other.minute_ && second_ >= other.second_ &&
			msecond_ >= other.msecond_;
}


bool CalendarTime::isLeapYear() const
{
	return !(year_ % 4) && ((year_ % 100) || !(year_ % 400));
}


int CalendarTime::year() const
{
	return year_;
}


int CalendarTime::month() const
{
	return month_;
}


int CalendarTime::week() const
{
	return 0;
}


int CalendarTime::weekDay() const
{
	return weekDay_;
}


int CalendarTime::monthDay() const
{
	return monthDay_;
}


int CalendarTime::yearDay() const
{
	return yearDay_;
}


void CalendarTime::setDate(int monthDay, int month, i16 year)
{
	year_ = year;
	month_ = bound(1, month, 12);

	const int* offsetTable;
	const int* countTable;

	if(isLeapYear()) {
		countTable = kDaysPerMonthLeap;
		offsetTable = kDaysPerMonthLeap;
	}
	else {
		countTable = kDaysPerMonth;
		offsetTable = kDaysPerMonth;
	}

	monthDay_ = bound(1, monthDay, offsetTable[month_-1]);

	yearDay_ = std::accumulate(countTable, countTable + month_ - 1,
			monthDay_ + 1);

	weekDay_ = weekDayOf(monthDay_, month_, year_);
}


int CalendarTime::hour() const
{
	return hour_;
}


int CalendarTime::minute() const
{
	return minute_;
}


int CalendarTime::second() const
{
	return second_;
}


int CalendarTime::msecond() const
{
	return msecond_;
}


void CalendarTime::setTime(int hour, int minute, int second, int msecond)
{
	hour_ = bound(0, hour, 23);
	minute_ = bound(0, minute, 59);
	second_ = bound(0, second, 59);
	msecond_ = bound(0, msecond, 999);
}


Duration CalendarTime::toDuration() const
{
	Duration result;
	int years = year_;

	result += Duration(years / 400 * kMsecsPer400Years);
	years %= 400;

	result += Duration(years / 100 * kMsecsPer100Years);
	years %= 100;

	result += Duration(years / 4 * kMsecsPer4Years);
	result += Duration(years % 4 * Duration::kMsecsPerYear);
	result += Duration((yearDay_ - 1) * Duration::kMsecsPerDay);
	result += Duration(hour_ * Duration::kMsecsPerHour);
	result += Duration(minute_ * Duration::kMsecsPerMinute);
	result += Duration(second_ * Duration::kMsecsPerSecond);
	result += Duration(msecond_);

	return result - Duration(kEpochMsecs);
}


int CalendarTime::weekDayOf(int day, int month, int year)
{
	static int table[Duration::kMonthsPerYear] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

	if(month < 3)
		year--;

	return (year + year / 4 - year / 100 + year / 400 + table[month-1] + day)
			% Duration::kDaysPerWeek;
}


} // namespace Tech
