#ifndef TECH_SYSTEM_TIMEZONE_LINUX_H
#define TECH_SYSTEM_TIMEZONE_LINUX_H

#include <vector>
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
	struct Transition {
		Duration timestamp;
		Duration utcOffset;
		bool isDst;
	};

	struct Comparator {
		bool operator()(const Transition& t, const Duration& d) const
		{
			return t.timestamp < d;
		}

		bool operator()(const Duration& d, const Transition& t) const
		{
			return d < t.timestamp;
		}
	};

	static const Binary tzDir_;
	static Binary local_;
	Binary name_;
	std::vector<Transition> transitions_;

	bool parseTimeZoneFile(const Binary& path);

	static Binary tzDir();

	static void scanDirectory(const Binary& path, uint prefixLength,
			BinaryList* result);

	static Binary readTimeZone();
	static Binary readLocalTime();
};


} // namespace Tech


#endif // TECH_SYSTEM_TIMEZONE_LINUX_H
