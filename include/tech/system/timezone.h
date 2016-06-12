#ifndef TECH_SYSTEM_TIMEZONE_H
#define TECH_SYSTEM_TIMEZONE_H

#include <map>
#include <tech/bytearray.h>
#include <tech/duration.h>
#include <tech/pimpl.h>


namespace Tech {


class TimeZoneImpl;


class TimeZone final : public Interface<TimeZoneImpl> {
public:
	TimeZone(const ByteArray& name);

	ByteArray name() const;
	Duration fromUtc(const Duration& utc) const;
	Duration toUtc(const Duration& local, bool isDst = false) const;

	static TimeZone local();
	static TimeZone utc();
	static ByteArrayList availableTimeZones();

private:
	// Time zone cache
	static std::map<ByteArray, Arc<TimeZoneImpl>> dataByName_;

	static Arc<TimeZoneImpl> implForZone(const ByteArray& name);
};


} // namespace Tech


#endif // TECH_SYSTEM_TIMEZONE_H
