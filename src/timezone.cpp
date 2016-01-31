#include <tech/timezone.h>

#include <tech/platform.h>
#include PLATFORM_HEADER(timezone)


namespace Tech {


std::map<ByteArray, Arc<TimeZoneImpl>> TimeZone::dataByName_;


TimeZone::TimeZone(const ByteArray& name) :
	Interface<TimeZoneImpl>(implForZone(name))
{
}


Arc<TimeZoneImpl> TimeZone::implForZone(const ByteArray& name)
{
	auto it = dataByName_.find(name);

	if(it == dataByName_.end()) {
		auto data = makeArc<TimeZoneImpl>(name);
		dataByName_.emplace(makePair(name, data));
		return data;
	}

	return it->second;
}


ByteArray TimeZone::name() const
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


ByteArrayList TimeZone::availableTimeZones()
{
	return TimeZoneImpl::availableTimeZones();
}


} // namespace Tech
