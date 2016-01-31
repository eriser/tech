#ifndef TECH_DELEGATE_H
#define TECH_DELEGATE_H

/**
 * Предназначение:
 * Delegate привязывается к статической функции или методу класса, после чего с помощью
 * Delegate::operator() позволяет вызвать эту функцию, передав ей необходимые аргументы,
 * и получить возвращенный результат. Фактически, можно считать Delegate указателем на
 * функцию обратного вызова.
 *
 * Мотивация:
 * Довольно часто возникает ситуация, когда обработка некоторого события должна
 * производиться вне текущего контекста. Например, класс кнопки может обнаружить факт ее
 * нажатия пользователем, но не может знать, как это нажатие обрабатывать. В идеале, за
 * логику поведения программы в зависимости от нажатия кнопок должна отвечать совершенно
 * другая, неизвестная классу кнопки сущность. Для уведомления внешнего объекта о
 * событии, достаточно, чтобы у этого объекта была функция-член с подходящей сигнатурой,
 * но совершенно не имеет значения, какой у него тип. В таких случаях обычно удобно
 * использовать функции обратного вызова (callbacks), но C++ накладывает существенные
 * ограничения при работе с указателями на функции-члены, т.к. считает разными указатели
 * на функции-члены с одинаковой сигнатурой у двух разных классов.
 *
 * Реализация:
 * Delegate выступает в роли посредника, храня внутри себя указатель на внешнюю функцию и
 * предоставляя интерфейс для ее вызова. Таким образом он легко может использоваться в
 * качестве "указателя" на функцию обратного вызова и при этом его применение не
 * ограничено исключительно статическими функциями.
 *
 * Пример использования:
 *
 * int f()
 * {
 *     return 20;
 * }
 *
 * class Foo {
 * public:
 *     void func(int value, const char* string)
 *     {
 *         // ...
 *     }
 * };
 *
 * Foo foo;
 *
 * Tech::Delegate<int()> delegate1(&f);
 * delegate1(); // Вызывает статическую функцию f()
 *
 * Tech::Delegate<void(int, const char*)> delegate2(&foo, &Foo::func);
 * delegate2(15, "test"); // Вызывает функцию-член foo.func(15, "test")
 *
 */

#include <cstring>
#include <map>
#include <tech/utils.h>


namespace internal {


class NoInheritA {};
class NoInheritB {};
class SingleInherit   : public NoInheritA {};
class MultipleInherit : public NoInheritA, public NoInheritB {};
class VirtualInherit  : virtual public NoInheritA {};
class Undefined;


} // namespace internal


namespace Tech {


// Разные компиляторы используют различное внутреннее представление указателя на
// функцию-член. kFunctionPointerMaxSize вычисляется на этапе компиляции и должна
// содержать максимально-возможный размер указателя на функцию для используемого
// компилятора.
static constexpr size_t kFunctionPointerMaxSize = maxOf(
		sizeof(void (*)()),
		sizeof(void(internal::NoInheritA::*)()),
		sizeof(void(internal::SingleInherit::*)()),
		sizeof(void(internal::MultipleInherit::*)()),
		sizeof(void(internal::VirtualInherit::*)()),
		sizeof(void(internal::Undefined::*)())
);


template<typename>
class Delegate;


template<typename R, typename ...A>
class Delegate<R(A...)> {
public:
	/**
	 * Создает пустой делегат.
	 */
	Delegate();

	/**
	 * Создает делегат и привязывает его к статической функции @p function.
	 */
	Delegate(R(*function)(A...));

	/**
	 * Создает делегат и привязывает его к неконстантной функции-члену @p function
	 * объекта @p target.
	 */
	template<typename T, typename B, EnableIf<
			std::is_base_of<B, T>>...>
	Delegate(T* target, R(B::*function)(A...));

	/**
	 * Создает делегат и привязывает его к константной функции-члену @p function объекта
	 * @p target.
	 */
	template<typename T, typename B, EnableIf<
			std::is_base_of<B, T>>...>
	Delegate(T* target, R(B::*function)(A...) const);

	/**
	 * Вызывает привязанную функцию, передавая ей @c args в качестве аргументов.
	 */
	R operator()(A... args) const;

	/**
	 * Возвращает @c true, если делегат равен @p other.
	 */
	bool operator==(const Delegate& other) const;

	/**
	 * Возвращает @c true, если делегат не равен @p other.
	 */
	bool operator!=(const Delegate& other) const;

	/**
	 * Возвращает @c true, если делегат "меньше" @p other. Функция требуется для
	 * использования делегатов с ассоциативными контейнерами типа std::set или std::map.
	 */
	bool operator<(const Delegate& other) const;

	/**
	 * Возвращает @c true, если делегат пустой.
	 */
	bool operator!() const;

	/**
	 * Возвращает @c true, если делегат пустой.
	 */
	bool isNull() const;

	/**
	 * Возвращает указатель на начало данных объекта, с методом которого связан делегат.
	 * В случае, если делегат пустой или связан со статической функцией, возвращается
	 * @c nullptr.
	 */
	void* target() const;

private:
	// Структура для хранения значения указателя на функцию.
	// C++ не позволяет выполнять приведение типа указателя на функцию-член к какому-либо
	// другому типу данных, но позволяет преобразовывать тип указателя на шаблонный
	// класс, параметризированный указателем на функцию-член. За счет этого появляется
	// возможность сохранить и восстановить указатель на функцию с любой сигнатурой.
	template<typename T>
	struct PointerWrapper {
		PointerWrapper(T data) : value(data) { }
		T value;
	};

	// Указатель на объект, к методу которого привязан делегат. При связи со статической
	// функцией содержит nullptr. Если значение равно this, то это интерпретируется как
	// признак пустого делегата. Такое поведение обусловлено оптимизацией, чтобы избежать
	// сравнения всего массива storage_ с нулем в методе isNull() или использования
	// дополнительной памяти для хранения признака пустого делегата. Значение this
	// выбрано в качестве специального, потому что нет смысла присоединять делегат к
	// самому себе, т.к. это приведет к бесконечной рекурсии.
	void* target_;

	// Буфер, хранящий указатель на функцию
	u8 storage_[kFunctionPointerMaxSize];

	// Указатель на функцию-трамплин, которая восстанавливает тип и осуществляет вызов
	// привязанной функции
	using CallerProc = R(*)(const Delegate<R(A...)>*, A&&...);
	CallerProc callerProc_ { };


	// Трамплин для вызова статической функции
	template<typename F>
	static R functionCaller(const Delegate<R(A...)>* self, A&&... args)
	{
		auto pointerData = reinterpret_cast<const PointerWrapper<F>*>(self->storage_);
		return pointerData->value(std::forward<A>(args)...);
	}

	// Трамплин для вызова функции-члена
	template<typename C, typename F>
	static R memberCaller(const Delegate<R(A...)>* self, A&&... args)
	{
		auto target = reinterpret_cast<C*>(self->target_);
		auto pointerData = reinterpret_cast<const PointerWrapper<F>*>(self->storage_);
		return (target->*pointerData->value)(std::forward<A>(args)...);
	}
};


template<typename R, typename ...A>
Delegate<R (A...)>::Delegate() :
	target_(this), // (оптимизация) this - признак пустого делегата, см. описание target_
	storage_{ 0 }
{
}


template<typename R, typename ...A>
Delegate<R(A...)>::Delegate(R(*function)(A...)) :
	target_(nullptr),
	storage_{ 0 }
{
	using Function = R(*)(A...);

	new (storage_) PointerWrapper<Function>(function);
	callerProc_ = functionCaller<Function>;
}


template<typename R, typename ...A>
template<typename T, typename B, EnableIf<
		std::is_base_of<B, T>>...>
Delegate<R(A...)>::Delegate(T* target, R(B::*function)(A...)) :
	target_(target),
	storage_{ 0 }
{
	using Function = R(B::*)(A...);

	new (storage_) PointerWrapper<Function>(function);
	callerProc_ = memberCaller<T, Function>;
}


template<typename R, typename ...A>
template<typename T, typename B, EnableIf<
		std::is_base_of<B, T>>...>
Delegate<R(A...)>::Delegate(T* target, R(B::*function)(A...) const) :
	target_(target),
	storage_{ 0 }
{
	using Function = R(B::*)(A...) const;

	new (storage_) PointerWrapper<Function>(function);
	callerProc_ = memberCaller<T, Function>;
}


template<typename R, typename ...A>
R Delegate<R(A...)>::operator()(A... args) const
{
	return callerProc_(this, std::forward<A>(args)...);
}


template<typename R, typename ...A>
bool Delegate<R(A...)>::operator==(const Delegate& other) const
{
	if(target() == other.target())
		return std::memcmp(storage_, other.storage_, kFunctionPointerMaxSize) == 0;

	return false;
}


template<typename R, typename ...A>
bool Delegate<R(A...)>::operator!=(const Delegate& other) const
{
	return !(*this == other);
}


template<typename R, typename ...A>
bool Delegate<R(A...)>::operator<(const Delegate& other) const
{
	if(target() < other.target())
		return true;

	return std::memcmp(storage_, other.storage_, kFunctionPointerMaxSize) < 0;
}


template<typename R, typename ...A>
bool Delegate<R(A...)>::operator!() const
{
	return isNull();
}


template<typename R, typename ...A>
bool Delegate<R(A...)>::isNull() const
{
	// (оптимизация) this - признак пустого делегата, см. описание target_
	return target_ == static_cast<const void*>(this);
}


template<typename R, typename ...A>
void* Delegate<R(A...)>::target() const
{
	if(isNull())
		return nullptr;

	return target_;
}


} // namespace Tech


#endif // TECH_DELEGATE_H
