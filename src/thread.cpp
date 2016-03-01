#include <tech/thread.h>

#include <thread>


namespace Tech {


Thread::Thread() :
	exitFlag_(false),
	state_(ThreadState::kStopped)
{
}


Thread::~Thread()
{
	// At this point part of the object is already destroyed (child destructors are
	// finished) so the virtual function table doesn't contain a proper run() pointer
	// anymore. Inform trampoline() about this fact through setting the flag.
	exitFlag_.test_and_set();

	waitForStarted();
	stop();
}


void Thread::start()
{
	if(state_ == ThreadState::kStopped) {
		state_ = ThreadState::kStarting;
		std::thread thread = std::thread(trampoline, this);
		thread.detach();
	}
}


void Thread::stop()
{
	if(state_ == ThreadState::kRunning)
		state_ = ThreadState::kStopping;
}


bool Thread::waitForStarted(const Duration& timeout)
{
	if(state_ == ThreadState::kStopping || state_ == ThreadState::kStopped)
		return false;

	std::cv_status status = std::cv_status::no_timeout;
	std::unique_lock<std::mutex> locker(guard_);

	while(state_ != ThreadState::kRunning) {
		if(state_ == ThreadState::kStopped)
			return false;

		if(timeout.isNull()) {
			event_.wait(locker);
		}
		else {
			auto duration = std::chrono::milliseconds(timeout.mseconds());
			status = event_.wait_for(locker, duration);
		}
	}

	return status == std::cv_status::no_timeout;
}


bool Thread::waitForFinished(const Duration& timeout)
{
	if(state_ == ThreadState::kStopped)
		return false;

	std::cv_status status = std::cv_status::no_timeout;
	std::unique_lock<std::mutex> locker(guard_);

	while(state_ != ThreadState::kStopped) {
		if(timeout.isNull()) {
			event_.wait(locker);
		}
		else {
			auto duration = std::chrono::milliseconds(timeout.mseconds());
			status = event_.wait_for(locker, duration);
		}
	}

	return status == std::cv_status::no_timeout;
}


bool Thread::isRunning() const
{
	return state_ == ThreadState::kRunning;
}


bool Thread::isFinished() const
{
	return state_ == ThreadState::kStopped;
}


ThreadState Thread::state() const
{
	return state_;
}


void Thread::trampoline(Thread* self)
{
	std::unique_lock<std::mutex> locker(self->guard_);
	self->state_ = ThreadState::kRunning;
	self->event_.notify_all();

	locker.unlock();

	// We can't call run() function if destructor is already called before we reached
	// this point
	if(!self->exitFlag_.test_and_set())
		self->run();

	locker.lock();
	self->state_ = ThreadState::kStopped;
	std::notify_all_at_thread_exit(self->event_, std::move(locker));
}


} // namespace Tech
