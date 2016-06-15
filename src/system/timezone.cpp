#include <tech/system/timezone.h>

#include <tech/platform.h>
#include PLATFORM_HEADER(timezone)


namespace Tech {


std::map<Binary, Arc<TimeZoneImpl>> TimeZone::dataByName_;


TimeZone::TimeZone(const Binary& name) :
	Interface<TimeZoneImpl>(implForZone(name))
{
}


Arc<TimeZoneImpl> TimeZone::implForZone(const Binary& name)
{
	auto it = dataByName_.find(name);

	if(it == dataByName_.end()) {
		auto data = makeArc<TimeZoneImpl>(name);
		dataByName_.emplace(makePair(name, data));
		return data;
	}

	return it->second;
}


Binary TimeZone::name() const
{
	return impl()->name();
}


Duration TimeZone::fromUtc(const Duration& utc) const
{
	return impl()->fromUtc(utc);
}


Duration TimeZone::toUtc(const Duration& local, bool isDst) const
{
	return impl()->toUtc(local, isDst);
}


TimeZone TimeZone::local()
{
	return TimeZoneImpl::local();
}


TimeZone TimeZone::utc()
{
	return TimeZoneImpl::utc();
}


BinaryList TimeZone::availableTimeZones()
{
	return TimeZoneImpl::availableTimeZones();
}


} // namespace Tech
