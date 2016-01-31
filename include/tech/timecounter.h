#ifndef TECH_TIMECOUNTER_H
#define TECH_TIMECOUNTER_H

#include <tech/duration.h>


namespace Tech {


class TimeCounter {
public:
	TimeCounter();

	Duration elapsed() const;
	bool hasExpired(Duration timeout) const;

	Duration restart();
	Duration durationTo(const TimeCounter& other);

private:
	Duration origin_;
};


inline
TimeCounter::TimeCounter() :
	origin_(0)
{
}


inline
Duration TimeCounter::elapsed() const
{
	return Duration::fromEpoch() - origin_;
}


inline
bool TimeCounter::hasExpired(Duration timeout) const
{
	return origin_ >= Duration::fromEpoch() + timeout;
}


inline
Duration TimeCounter::restart()
{
	Duration result = elapsed();
	origin_ += result;
	return result;
}


inline
Duration TimeCounter::durationTo(const TimeCounter& other)
{
	return other.origin_ - origin_;
}


} // namespace Tech


#endif // TECH_TIMECOUNTER_H
