#ifndef TECH_SYSTEM_TIMEZONE_LINUX_H
#define TECH_SYSTEM_TIMEZONE_LINUX_H

#include <vector>
#include <tech/system/timezone.h>


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

	static const ByteArray tzDir_;
	static ByteArray local_;
	ByteArray name_;
	std::vector<Transition> transitions_;

	bool parseTimeZoneFile(const ByteArray& path);

	static ByteArray tzDir();

	static void scanDirectory(const ByteArray& path, uint prefixLength,
			ByteArrayList* result);

	static ByteArray readTimeZone();
	static ByteArray readLocalTime();
};


} // namespace Tech


#endif // TECH_SYSTEM_TIMEZONE_LINUX_H
