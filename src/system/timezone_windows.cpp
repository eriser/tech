#include "timezone_windows.h"


namespace Tech {


TimeZoneImpl::TimeZoneImpl(const Binary& name) :
	Implementation<TimeZone>(nullptr)
{

}


Binary TimeZoneImpl::name() const
{
	// FIXME
	return Binary();
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


BinaryList TimeZoneImpl::availableTimeZones()
{
	// FIXME
	return BinaryList();
}


} // namespace Tech
