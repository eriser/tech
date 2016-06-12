#ifndef TECH_CALENDARTIME_H
#define TECH_CALENDARTIME_H

#include <tech/duration.h>
#include <tech/system/timezone.h>


namespace Tech {


enum class Month {
	kJanuary = 1,
	kFebrary,
	kMarch,
	kApril,
	kMay,
	kJune,
	kJule,
	kAugust,
	kSeptember,
	kOctober,
	kNovember,
	kDecember
};

enum class WeekDay {
	kSunday,
	kMonday,
	kTuesday,
	kWednesday,
	kThursday,
	kFriday,
	kSaturday,
};


class CalendarTime {
public:
	CalendarTime(int monthDay, int month, int year, int hour = 0, int minute = 0,
			int second = 0, int msecond = 0);

	CalendarTime(const Duration& duration);

	bool operator==(const CalendarTime& other) const;
	bool operator!=(const CalendarTime& other) const;
	bool operator<(const CalendarTime& other) const;
	bool operator>(const CalendarTime& other) const;
	bool operator<=(const CalendarTime& other) const;
	bool operator>=(const CalendarTime& other) const;

	bool isLeapYear() const;

	int year() const;
	int month() const;
	int week() const;
	int weekDay() const;
	int monthDay() const;
	int yearDay() const;

	void setDate(int monthDay, int month, i16 year);

	int hour() const;
	int minute() const;
	int second() const;
	int msecond() const;

	void setTime(int hour, int minute, int second, int msecond = 0);

	Duration toDuration() const;

private:
	// FIXME add an ability to represent B.C. dates (values should be signed)
	struct {
		uint year_     : 16;
		uint msecond_  : 10;
		uint second_   : 6;
		uint minute_   : 6;
		uint hour_     : 5;
		uint weekDay_  : 3;
		uint monthDay_ : 5;
		uint yearDay_  : 9;
		uint month_    : 4;
	};

	static int weekDayOf(int day, int month, int year);
};


} // namespace Tech


#endif // TECH_CALENDARTIME_H
