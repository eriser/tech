#include <tech/duration.h>

#include <chrono>


namespace Tech {


Duration::Duration(int hours, int minutes, int seconds, int mseconds)
{
	setDuration(hours, minutes, seconds, mseconds);
}


Duration::Duration(int weeks, int days, int hours, int minutes,	int seconds, int mseconds)
{
	setDuration(weeks, days, hours, minutes, seconds, mseconds);
}


void Duration::setDuration(int hours, int minutes, int seconds, int mseconds)
{
	msecs_ = hours   * kMsecsPerHour   +
			 minutes * kMsecsPerMinute +
			 seconds * kMsecsPerSecond + mseconds;
}


void Duration::setDuration(int weeks, int days, int hours, int minutes, int seconds,
		int mseconds)
{
	msecs_ = weeks   * kMsecsPerWeek   +
			 days    * kMsecsPerDay    +
			 hours   * kMsecsPerHour   +
			 minutes * kMsecsPerMinute +
			 seconds * kMsecsPerSecond + mseconds;
}


bool Duration::isNull() const
{
	return msecs_ == 0;
}


bool Duration::isNegative() const
{
	return msecs_ < 0;
}


int Duration::weeks() const
{
	return msecs_ / kMsecsPerWeek;
}


int Duration::days() const
{
	return msecs_ / kMsecsPerDay;
}


int Duration::hours() const
{
	return msecs_ / kMsecsPerHour;
}


i64 Duration::minutes() const
{
	return msecs_ / kMsecsPerMinute;
}


i64 Duration::seconds() const
{
	return msecs_ / kMsecsPerSecond;
}


i64 Duration::mseconds() const
{
	return msecs_;
}


Duration& Duration::operator+=(const Duration& other)
{
	msecs_ += other.msecs_;
	return *this;
}


Duration& Duration::operator-=(const Duration& other)
{
	msecs_ -= other.msecs_;
	return *this;
}


Duration& Duration::operator*=(int value)
{
	msecs_ *= value;
	return *this;
}


Duration& Duration::operator*=(float value)
{
	msecs_ *= value;
	return *this;
}


Duration& Duration::operator/=(int value)
{
	msecs_ /= value;
	return *this;
}


Duration& Duration::operator/=(float value)
{
	msecs_ /= value;
	return *this;
}


Duration& Duration::operator%=(int value)
{
	msecs_ %= value;
	return *this;
}


const Duration Duration::operator+(const Duration& other) const
{
	return Duration(*this) += other;
}


const Duration Duration::operator-(const Duration& other) const
{
	return Duration(*this) -= other;
}


const Duration Duration::operator*(int value) const
{
	return Duration(*this) *= value;
}


const Duration Duration::operator*(float value) const
{
	return Duration(*this) *= value;
}


const Duration Duration::operator/(int value) const
{
	return Duration(*this) /= value;
}


const Duration Duration::operator/(float value) const
{
	return Duration(*this) /= value;
}


const Duration Duration::operator%(int value) const
{
	return Duration(*this) %= value;
}


bool Duration::operator==(const Duration& other) const
{
	return msecs_ == other.msecs_;
}


bool Duration::operator!=(const Duration& other) const
{
	return msecs_ != other.msecs_;
}


bool Duration::operator<(const Duration& other) const
{
	return msecs_ < other.msecs_;
}


bool Duration::operator>(const Duration& other) const
{
	return msecs_ > other.msecs_;
}


bool Duration::operator<=(const Duration& other) const
{
	return msecs_ <= other.msecs_;
}


bool Duration::operator>=(const Duration& other) const
{
	return msecs_ >= other.msecs_;
}


bool Duration::operator!() const
{
	return msecs_ ==  0;
}


Duration Duration::fromEpoch()
{
	using std::chrono::system_clock;
	using std::chrono::duration_cast;
	using std::chrono::milliseconds;

	auto current = system_clock::now().time_since_epoch();
	return Duration(duration_cast<milliseconds>(current).count());
}


Duration Duration::second()
{
	return Duration(kMsecsPerSecond);
}


Duration Duration::minute()
{
	return Duration(kMsecsPerMinute);
}


Duration Duration::hour()
{
	return Duration(kMsecsPerHour);
}


Duration Duration::day()
{
	return Duration(kMsecsPerDay);
}


Duration Duration::week()
{
	return Duration(kMsecsPerWeek);
}


Duration Duration::year()
{
	return Duration(kDaysPerYear * kMsecsPerDay);
}


Duration Duration::leapYear()
{
	return Duration(kDaysPerLeapYear * kMsecsPerDay);
}


Duration Duration::seconds(i64 count)
{
	return Duration(kMsecsPerSecond * count);
}


Duration Duration::minutes(i64 count)
{
	return Duration(kMsecsPerMinute * count);
}


Duration Duration::hours(i64 count)
{
	return Duration(kMsecsPerHour * count);
}


Duration Duration::days(i64 count)
{
	return Duration(kMsecsPerDay * count);
}


const Duration operator*(int value, const Duration& other)
{
	return other * value;
}


const Duration operator*(float value, const Duration& other)
{
	return other * value;
}


Duration Duration::operator-()
{
	return Duration(-msecs_);
}


} // namespace Tech
