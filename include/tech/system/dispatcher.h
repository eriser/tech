#ifndef TECH_SYSTEM_DISPATCHER_H
#define TECH_SYSTEM_DISPATCHER_H

#include <tech/delegate.h>
#include <tech/flags.h>
#include <tech/pimpl.h>
#include <tech/signal.h>


namespace Tech {


class DispatcherImpl;


enum class EventMask {
	kRead  = 0x01,
	kWrite = 0x02,
	kClose = 0x04,
	kError = 0x08
};


using EventHandler = Delegate<void(int, EventMask)>;


class Dispatcher : public Trackable, public Interface<DispatcherImpl> {
public:
	static Dispatcher* instance();

	Dispatcher(const Dispatcher&) = delete;
	Dispatcher& operator=(const Dispatcher&) = delete;

	void processEvents();
	void stopProcessing();

	bool isHandlerRegistered(int fd) const;
	bool registerHandler(int fd, Flags<EventMask> mask, const EventHandler& handler);
	bool deregisterHandler(int fd);


	class AbstractDeleter {
	public:
		virtual ~AbstractDeleter() = default;
		virtual void deleteObject() = 0;
	};

	template<class C>
	class Deleter : public AbstractDeleter {
	public:
		Deleter(C* ptr) :
			ptr_(ptr)
		{
		}

		~Deleter()
		{
			if(ptr_)
				deleteObject();
		}

		void deleteObject() override
		{
			delete ptr_;
			ptr_ = nullptr;
		}

	private:
		C* ptr_;
	};

	template<typename T>
	void scheduleDeletion(T* object)
	{
		addDeleter(new Deleter<T>(object));
	}

private:
	Dispatcher();
	void addDeleter(AbstractDeleter* deleter);
};


} // namespace Tech


DECLARE_FLAG_OPERATORS(Tech::EventMask);


#endif // TECH_SYSTEM_DISPATCHER_H
