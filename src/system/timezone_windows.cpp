#include "timezone_windows.h"


namespace Tech {


TimeZoneImpl::TimeZoneImpl(const ByteArray& name) :
	Implementation<TimeZone>(nullptr)
{

}


ByteArray TimeZoneImpl::name() const
{
	// FIXME
	return ByteArray();
}


Duration TimeZoneImpl::fromUtc(const Duration& utc) const
{
	// FIXME
	return Duration();
}


Duration TimeZoneImpl::toUtc(const Duration& local, bool isDst) const
{
	// FIXME
	return Duration();
}


TimeZone TimeZoneImpl::local()
{
	// FIXME
	return TimeZone("");
}


TimeZone TimeZoneImpl::utc()
{
	// FIXME
	return TimeZone("");
}


ByteArrayList TimeZoneImpl::availableTimeZones()
{
	// FIXME
	return ByteArrayList();
}


} // namespace Tech
