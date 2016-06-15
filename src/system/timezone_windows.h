#ifndef TECH_SYSTEM_TIMEZONE_WINDOWS_H
#define TECH_SYSTEM_TIMEZONE_WINDOWS_H

#include <tech/system/timezone.h>


namespace Tech {


class TimeZoneImpl : public Implementation<TimeZone> {
public:
	TimeZoneImpl(const Binary& name);

	Binary name() const;
	Duration fromUtc(const Duration& utc) const;
	Duration toUtc(const Duration& local, bool isDst) const;

	static TimeZone local();
	static TimeZone utc();
	static BinaryList availableTimeZones();

private:

};


} // namespace Tech


#endif // TECH_SYSTEM_TIMEZONE_WINDOWS_H
