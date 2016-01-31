#ifndef TECH_SEMAPHORE_H
#define TECH_SEMAPHORE_H

#include <condition_variable>
#include <mutex>
#include <tech/duration.h>


namespace Tech {


class Semaphore {
public:
	using Handle = std::condition_variable::native_handle_type;

	explicit Semaphore(size_t count = 0);

	Semaphore(const Semaphore&) = delete;
	Semaphore& operator=(const Semaphore&) = delete;

	void post();
	bool wait(const Duration& duration = Duration());
	bool waitUntil(const Duration& timestamp);

	Handle nativeHandle();

private:
	size_t count_;
	std::mutex mutex_;
	std::condition_variable cv_;
};


inline
Semaphore::Semaphore(size_t count) :
	count_(count)
{
}


inline
void Semaphore::post()
{
	std::lock_guard<std::mutex> lock(mutex_);
	++count_;
	cv_.notify_one();
}


inline
bool Semaphore::wait(const Duration& duration)
{
	std::unique_lock<std::mutex> lock(mutex_);
	bool complete;

	if(!duration.isNull()) {
		std::chrono::milliseconds msecs(duration.mseconds());
		complete = cv_.wait_for(lock, msecs, [&]{ return count_ > 0; });
	}
	else {
		cv_.wait(lock, [&]{ return count_ > 0; });
		complete = true;
	}

	if(complete)
		--count_;

	return complete;
}


inline
bool Semaphore::waitUntil(const Duration& timestamp)
{
	std::unique_lock<std::mutex> lock(mutex_);
	std::chrono::milliseconds ts(timestamp.mseconds());
	auto finished = cv_.wait_for(lock, ts, [&]{ return count_ > 0; });

	if(finished)
		--count_;

	return finished;
}


inline
Semaphore::Handle Semaphore::nativeHandle()
{
	return cv_.native_handle();
}


} // namespace Tech


#endif // TECH_SEMAPHORE_H
