#ifndef TECH_THREAD_H
#define TECH_THREAD_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <tech/duration.h>
#include <tech/types.h>


namespace Tech {


enum class ThreadState {
	kStopped,
	kStarting,
	kRunning,
	kStopping
};


class Thread {
public:
	Thread();
	virtual ~Thread();

	void start();
	void stop();

	bool waitForStarted(const Duration& timeout = Duration());
	bool waitForFinished(const Duration& timeout = Duration());

	bool isRunning() const;
	bool isFinished() const;

protected:
	ThreadState state() const;
	virtual void run() = 0;

private:
	std::atomic_flag exitFlag_;
	std::atomic<ThreadState> state_;
	mutable std::mutex guard_;
	std::condition_variable event_;

	static void trampoline(Thread* self);
};


} // namespace Tech


#endif // TECH_THREAD_H
