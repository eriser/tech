#ifndef TECH_SYSTEM_TIMEZONE_H
#define TECH_SYSTEM_TIMEZONE_H

#include <map>
#include <tech/binary.h>
#include <tech/duration.h>
#include <tech/pimpl.h>


namespace Tech {


class TimeZoneImpl;


class TimeZone final : public Interface<TimeZoneImpl> {
public:
	TimeZone(const Binary& name);

	Binary name() const;
	Duration fromUtc(const Duration& utc) const;
	Duration toUtc(const Duration& local, bool isDst = false) const;

	static TimeZone local();
	static TimeZone utc();
	static BinaryList availableTimeZones();

private:
	// Time zone cache
	static std::map<Binary, Arc<TimeZoneImpl>> dataByName_;

	static Arc<TimeZoneImpl> implForZone(const Binary& name);
};


} // namespace Tech


#endif // TECH_SYSTEM_TIMEZONE_H
