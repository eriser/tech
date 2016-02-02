#ifndef TECH_TIMEZONE_WINDOWS_H
#define TECH_TIMEZONE_WINDOWS_H

#include <tech/timezone.h>


namespace Tech {


class TimeZoneImpl : public Implementation<TimeZone> {
public:
	TimeZoneImpl(const ByteArray& name);

	ByteArray name() const;
	Duration fromUtc(const Duration& utc) const;
	Duration toUtc(const Duration& local, bool isDst) const;

	static TimeZone local();
	static TimeZone utc();
	static ByteArrayList availableTimeZones();

private:

};


} // namespace Tech


#endif // TECH_TIMEZONE_WINDOWS_H
