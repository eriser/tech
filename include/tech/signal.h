#ifndef TECH_SIGNAL_H
#define TECH_SIGNAL_H

#include <vector>
#include <tech/delegate.h>

#ifdef signals
#undef signals
#endif
#define signals public


namespace Tech {


class TrackableWatcher;


/**
 * Классы, наследующие Trackable, при своем перемещении или уничтожении уведомляют об
 * этом подключенные к их методам сигналы и таким образом позволяют автоматизировать
 * обработку этих событий. При перемещении или удалении объектов классов, не наследующих
 * Trackable, требуется вручную отслеживать подключенные к их методам сигналы. В
 * противном случае, генерация подключенного к более невалидному объекту сигнала приведет
 * к undefined behaviour.
 */
class Trackable {
public:
	Trackable() = default;

	Trackable(const Trackable& other);

	Trackable(Trackable&& other);

	virtual ~Trackable();

	Trackable& operator=(const Trackable& other);

	void registerWatcher(TrackableWatcher* watcher);

	void unregisterWatcher(TrackableWatcher* watcher);

	void updateWatcher(TrackableWatcher* old, TrackableWatcher* watcher);

private:
	std::map<TrackableWatcher*, int> watchers_;
};


/**
 * Наблюдатель за объектами-потомками Trackable.
 */
class TrackableWatcher {
public:
	virtual ~TrackableWatcher() = default;
	virtual void destroyEvent(Trackable* trackable) = 0;
	virtual void moveEvent(Trackable* old, Trackable* trackable) = 0;
};


/**
 * @tparam T Сигнатура функции, к которой будет подключаться сигнал.
 * @tparam K Ключ доступа (Key<TypeName>), с помощью которого можно ограничить список
 *           типов, которым разрешено делать генерацию сигнала.
 */
template<typename T, typename K = void>
class Signal;


template<typename R, typename K, typename ...A>
class Signal<R(A...), K> : public TrackableWatcher {
public:
	/**
	 * Создает пустой сигнал с сигнатурой @p R(A...) и (опционально) ключом доступа @p K.
	 */
	Signal<R(A...), K>() = default;

	/**
	 * Создает пустой сигнал при копировании (соединения не копируются).
	 */
	Signal<R(A...), K>(const Signal<R(A...), K>& other);

	/**
	 * Создает сигнал и перемещает в него все соединения из @p other.
	 */
	Signal<R(A...), K>(Signal<R(A...), K>&& other);

	/**
	 * Уничтожает сигнал, при этом происходит отключение всех соединенных функций.
	 */
	~Signal();

	/**
	 *
	 */
	Signal<R(A...), K>& operator=(const Signal<R(A...), K>& other);

	/**
	 * Производит вызов всех подключенных функций, передавая им @p args в качестве
	 * аргументов. Данная версия функции присутствует только когда параметром шаблона
	 * @p R является @c void и отсутствует ключ доступа @p K.
	 */
	template<typename T1 = R, typename T2 = K, EnableIf<
			std::is_void<T1>,
			std::is_void<T2>>...>
	void operator()(A... args) const;

	/**
	 * Производит вызов всех подключенных функций, передавая им @p args в качестве
	 * аргументов и возвращает значение, которое вернет последняя из вызванных функций.
	 * Данная версия функции присутствует только когда параметром шаблона @p R не
	 * является @c void и отсутствует ключ доступа @p K.
	 */
	template<typename T1 = R, typename T2 = K, EnableIf<
			Not<std::is_void<T1>>,
			std::is_void<T2>>...>
	R operator()(A... args) const;

	/**
	 * Производит вызов всех подключенных функций, передавая им @p args в качестве
	 * аргументов. Данная версия функции присутствует только когда задан ключ доступа
	 * @p K при инстанцировании шаблона @c Signal и параметром шаблона функции
	 * @c operator() @p R является @c void.
	 */
	template<typename T1 = R, typename T2 = K, EnableIf<
			std::is_void<T1>,
			Not<std::is_void<T2>>>...>
	void operator()(T2 key, A... args) const;

	/**
	 * Производит вызов всех подключенных функций, передавая им @p args в качестве
	 * аргументов и возвращает значение, которое вернет последняя из вызванных функций.
	 * Данная версия функции присутствует только когда задан ключ доступа @p K
	 * при инстанцировании шаблона @c Signal и параметром шаблона функции @c operator()
	 * @p R не является @c void.
	 */
	template<typename T1 = R, typename T2 = K, EnableIf<
			Not<std::is_void<T1>>,
			Not<std::is_void<T2>>>...>
	R operator()(T2 key, A... args) const;

	/**
	 * Возвращает @c true, если сигнал подключен хотя бы к одной функции.
	 */
	bool hasConnections() const;

	/**
	 * Подключает статическую функцию @p function.
	 */
	void connect(R(*function)(A...));

	/**
	 * Подключает неконстантную функцию-член @p function объекта @p target.
	 */
	template<typename T, typename B, EnableIf<
			Not<std::is_base_of<Trackable, T>>,
			std::is_base_of<B, T>>...>
	void connect(T* target, R(B::*function)(A...));

	/**
	 * Подключает константную функцию-член @p function объекта @p target.
	 */
	template<typename T, typename B, EnableIf<
			std::is_base_of<Trackable, T>,
			std::is_base_of<B, T>>...>
	void connect(T* target, R(B::*function)(A...) const);

	/**
	 * Подключает неконстантную функцию-член @p function отслеживаемого (Trackable)
	 * объекта @p target.
	 */
	template<typename T, typename B, EnableIf<
			std::is_base_of<Trackable, T>,
			std::is_base_of<B, T>>...>
	void connect(T* target, R(B::*function)(A...));

	/**
	 * Подключает константную функцию-член @p function отслеживаемого (Trackable) объекта
	 * @p target.
	 */
	template<typename T, typename B, EnableIf<
			Not<std::is_base_of<Trackable, T>>,
			std::is_base_of<B, T>>...>
	void connect(T* target, R(B::*function)(A...) const);

	/**
	 * Отключает все подключенные функции.
	 */
	void disconnect();

	/**
	 * Отключает подключенную статическую функцию @p function.
	 */
	void disconnect(R (*function)(A...));

	/**
	 * Отключает подключенную неконстантную функцию-член @p function объекта @p target.
	 */
	template<typename T>
	void disconnect(T* target, R(T::*function)(A...));

	/**
	 * Отключает подключенную константную функцию-член @p function объекта @p target.
	 */
	template<typename T>
	void disconnect(T* target, R(T::*function)(A...) const);

	/**
	 * Отключает все подключенные функции-члены объекта @p target.
	 */
	template<typename T>
	void disconnect(T* target);

private:
	using DelegateType = Delegate<R(A...)>;
	using SignalType = Signal<R(A...), K>;

	struct Info {
		int isTrackable : 1;  // Признак связанности с методом потомка Trackable
		int adjustment  : 15; // Смещение до начала структуры Trackable
	};

	std::vector<Pair<DelegateType, Info>> slots_;

	/**
	 * Анализирует соединение с индексом @p index и, если это возможно, выполняет
	 * пеобразование и возвращает указатель на объект отслеживаемого класса (Trackable).
	 * В случае, если соединение с индексом @p index не является соединением с методом
	 * отслеживаемого класса (Trackable), возвращается @c nullptr.
	 * Проверка корректности значения @p index не производится, т.к. эта функция не
	 * является частью публичного интерфейса.
	 */
	Trackable* getTrackable(int index) const;

	void doDisconnect(const Delegate<R(A...)>& delegate);

	void destroyEvent(Trackable* trackable);
	void moveEvent(Trackable* old, Trackable* newTrackable);
};


inline
Trackable::Trackable(const Trackable&)
{
	// При копировании объектов Trackable производить копирование установленных связей с
	// сигналами не требуется, поэтому здесь мы ничего не делаем
}


inline
Trackable::Trackable(Trackable&& other)
{
	// При перемещении уведомляем все подключенные сигналы об изменении нашего адреса
	for(auto& it : other.watchers_)
		it.first->moveEvent(&other, this);

	watchers_.swap(other.watchers_);
}


inline
Trackable::~Trackable()
{
	while(!watchers_.empty()) {
		auto it = watchers_.begin();

		// watcher->destroyEvent() производит вызов unregisterWatcher()
		it->first->destroyEvent(this);
	}
}


inline
Trackable& Trackable::operator=(const Trackable&)
{
	// При присваивании объектов Trackable производить копирование установленных связей с
	// сигналами не требуется, поэтому здесь мы ничего не делаем
	return *this;
}


inline
void Trackable::registerWatcher(TrackableWatcher* watcher)
{
	auto it = watchers_.find(watcher);
	if(it == watchers_.end())
		watchers_.emplace(std::make_pair(watcher, 1));
	else
		it->second++;
}


inline
void Trackable::unregisterWatcher(TrackableWatcher* watcher)
{
	auto it = watchers_.find(watcher);
	if(it != watchers_.end()) {
		it->second--;
		if(it->second == 0)
			watchers_.erase(it);
	}
}


inline
void Trackable::updateWatcher(TrackableWatcher* old, TrackableWatcher* watcher)
{
	auto it = watchers_.find(old);
	if(it != watchers_.end()) {
		int count = it->second;
		watchers_.erase(it);
		watchers_.emplace(std::make_pair(watcher, count));
	}
}


template<typename R, typename K, typename ...A>
Signal<R(A...), K>::Signal(const Signal<R(A...), K>&)
{
	// При копировании сигналов копировать подключения не требуется, поэтому здесь мы
	// ничего не делаем
}


template<typename R, typename K, typename ...A>
Signal<R(A...), K>::Signal(Signal<R(A...), K>&& other)
{
	// При перемещении уведомляем все подключенные trackable объекты об изменении адреса
	// сигнала
	for(int i = 0; i < slots_.size(); ++i) {
		Trackable* trackable = getTrackable(i);
		if(trackable)
			trackable->updateWatcher(&other, this);
	}

	slots_.swap(other.slots_);
}


template<typename R, typename K, typename ...A>
Signal<R(A...), K>::~Signal()
{
	disconnect();
}


template<typename R, typename K, typename ...A>
Signal<R(A...), K>& Signal<R(A...), K>::operator=(const Signal<R(A...), K>& other)
{
	*this = other;
	return *this;
}


template<typename R, typename K, typename ...A>
template<typename T1, typename T2, EnableIf<
		std::is_void<T1>,
		std::is_void<T2>>...>
void Signal<R(A...), K>::operator()(A... args) const
{
	for(auto& slot : slots_)
		slot.first(args...);
}


template<typename R, typename K, typename ...A>
template<typename T1, typename T2, EnableIf<
		Not<std::is_void<T1>>,
		std::is_void<T2>>...>
R Signal<R(A...), K>::operator()(A... args) const
{
	R result = R();

	for(auto& slot : slots_)
		result = slot.first(args...);

	return result;
}


template<typename R, typename K, typename ...A>
template<typename T1, typename T2, EnableIf<
		std::is_void<T1>,
		Not<std::is_void<T2>>>...>
void Signal<R(A...), K>::operator()(T2 key, A... args) const
{
	for(auto& slot : slots_)
		slot.first(args...);
}


template<typename R, typename K, typename ...A>
template<typename T1, typename T2, EnableIf<
		Not<std::is_void<T1>>,
		Not<std::is_void<T2>>>...>
R Signal<R(A...), K>::operator()(T2 key, A... args) const
{
	R result = R();

	for(auto& slot : slots_)
		result = slot.first(args...);

	return result;
}


template<typename R, typename K, typename ...A>
bool Signal<R(A...), K>::hasConnections() const
{
	return !slots_.empty();
}


template<typename R, typename K, typename ...A>
void Signal<R(A...), K>::connect(R (*function)(A...))
{
	slots_.emplace_back(makePair(DelegateType(function), Info()));
}


template<typename R, typename K, typename ...A>
template<typename T, typename B, EnableIf<
		Not<std::is_base_of<Trackable, T>>,
		std::is_base_of<B, T>>...>
void Signal<R(A...), K>::connect(T* target, R(B::*function)(A...))
{
	Info info;
	info.isTrackable = false;
	slots_.emplace_back(makePair(DelegateType(target, function), info));
}


template<typename R, typename K, typename ...A>
template<typename T, typename B, EnableIf<
		Not<std::is_base_of<Trackable, T>>,
		std::is_base_of<B, T>>...>
void Signal<R(A...), K>::connect(T* target, R(B::*function)(A...) const)
{
	Info info;
	info.isTrackable = false;
	slots_.emplace_back(makePair(DelegateType(target, function), info));
}


template<typename R, typename K, typename ...A>
template<typename T, typename B, EnableIf<
		std::is_base_of<Trackable, T>,
		std::is_base_of<B, T>>...>
void Signal<R(A...), K>::connect(T* target, R(B::*function)(A...))
{
	Info info;
	info.isTrackable = true;
	info.adjustment = reinterpret_cast<u8*>(static_cast<Trackable*>(target)) -
			  reinterpret_cast<u8*>(target);

	target->registerWatcher(this);
	slots_.emplace_back(makePair(DelegateType(target, function), info));
}


template<typename R, typename K, typename ...A>
template<typename T, typename B, EnableIf<
		std::is_base_of<Trackable, T>,
		std::is_base_of<B, T>>...>
void Signal<R(A...), K>::connect(T* target, R(B::*function)(A...) const)
{
	Info info;
	info.isTrackable = true;
	info.adjustment = reinterpret_cast<u8*>(static_cast<Trackable*>(target)) -
					  reinterpret_cast<u8*>(target);

	target->registerWatcher(this);
	slots_.emplace_back(makePair(DelegateType(target, function), info));
}


template<typename R, typename K, typename ...A>
void Signal<R(A...), K>::disconnect()
{
	doDisconnect(DelegateType());
}


template<typename R, typename K, typename ...A>
void Signal<R(A...), K>::disconnect(R (*function)(A...))
{
	doDisconnect(DelegateType(function));
}


template<typename R, typename K, typename ...A>
template<typename T>
void Signal<R(A...), K>::disconnect(T* target, R(T::*function)(A...))
{
	doDisconnect(DelegateType(target, function));
}


template<typename R, typename K, typename ...A>
template<typename T>
void Signal<R(A...), K>::disconnect(T* target, R(T::*function)(A...) const)
{
	doDisconnect(DelegateType(target, function));
}


template<typename R, typename K, typename ...A>
template<typename T>
void Signal<R(A...), K>::disconnect(T* target)
{
	if(!target)
		return;

	for(int i = 0; i < slots_.size(); ++i) {
		if(slots_[i].first.target() == target) {
			Trackable* trackable = getTrackable(i);
			if(trackable)
				trackable->unregisterWatcher(this);

			slots_.erase(slots_.begin() + i);
			--i;
		}
	}
}


template<typename R, typename K, typename ...A>
Trackable* Signal<R(A...), K>::getTrackable(int index) const
{
	void* target = slots_[index].first.target();
	const Info& info = slots_[index].second;

	if(!info.isTrackable)
		return nullptr;

	void* result = static_cast<u8*>(target) + info.adjustment;
	return static_cast<Trackable*>(result);
}


template<typename R, typename K, typename ...A>
void Signal<R(A...), K>::doDisconnect(const Delegate<R(A...)>& delegate)
{
	bool isDisconnectAll = delegate.isNull();

	for(uint i = 0; i < slots_.size(); ++i) {
		if(isDisconnectAll || slots_[i].first == delegate) {
			Trackable* trackable = getTrackable(i);
			if(trackable)
				trackable->unregisterWatcher(this);

			slots_.erase(slots_.begin() + i);
			--i;
		}
	}
}


template<typename R, typename K, typename ...A>
void Signal<R(A...), K>::destroyEvent(Trackable* trackable)
{
	for(uint i = 0; i < slots_.size(); ++i) {
		Trackable* t = getTrackable(i);
		if(t == trackable) {
			trackable->unregisterWatcher(this);
			slots_.erase(slots_.begin() + i);
			--i;
		}
	}
}


template<typename R, typename K, typename ...A>
void Signal<R(A...), K>::moveEvent(Trackable* old, Trackable* newTrackable)
{
	for(uint i = 0; i < slots_.size(); ++i) {
		Trackable* trackable = getTrackable(i);
		if(trackable == old) {
			// TODO
			break;
		}
	}
}


} // namespace Tech


#endif // TECH_SIGNAL_H
