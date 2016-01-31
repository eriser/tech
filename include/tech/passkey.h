#ifndef TECH_PASSKEY_H
#define TECH_PASSKEY_H

/**
 * Мотивация:
 * В некоторых случаях требуется предоставить доступ к приватным полям класса для другого
 * класса. Данную проблему можно решить либо с помощью рефакторинга, либо объявив этот
 * класс как дружественный. Провести рефакторинг по разным причинам не всегда
 * представляется возможным, а использование дружественного класса технически позволит
 * ему получить доступ к любым приватным полям, что приведет к нарушению инкапсуляции и
 * усложнению связей между классами. Идиома Passkey позволяет ограничить набор типов,
 * которым будет разрешено выполнить вызов конкретного метода. Таким образом становится
 * возможным организовать для требуемых типов индивидуальные интерфейсы и при этом
 * избежать нарушения инкапсуляции.
 *
 * Реализация:
 * В каждый метод, к которому требуется ограничить доступ ("ограниченный" метод),
 * добавляется один дополнительный аргумент шаблонного класса PassKey<> и при этом сам
 * метод делается публичным. В качестве аргементов, шаблону PassKey добавляются имена
 * типов, которым требуется разрешить вызов данного метода. Key объявляет все свои
 * аргументы дружественными классам и имеет приватный конструктор. За счет этого, создать
 * экземпляр класса PassKey можно только внутри метода дружественного класса и таким
 * образом получается организовать управление правами доступа. Шаблонный класс PassKey
 * реализован как набор частичных специализаций, т.к. нет возможности использовать
 * variadic templates (при использовании variadic templates требуется производить
 * рекурсивное наследование, а отношение дружбы в C++ нетранзитивно).
 *
 * Достоинства:
 * - Возможность задания методу списка типов, которые могут его вызывать.
 *
 * Недостатки:
 * - Публичный интерфейс класса загромождается "ограниченными" методами
 * - "Ограниченным" методам требуется добавление дополнительного аргумента типа PassKey
 * - При вызове "ограниченного" метода требуется передавать дополнительный аргумент
 *
 * Пример:
 * class Foo {
 * public:
 *     void foo(PassKey<Bar> key)
 *     {
 *         // do something
 *     }
 * };
 *
 * class Bar {
 * public:
 *     void bar(Foo* foo)
 *     {
 *         foo->foo({});    // OK, Bar имеет право вызова метода Foo::foo()
 *     }
 * };
 *
 * class Other {
 * public:
 *     void other(Foo* foo)
 *     {
 *         foo->foo({});    // Ошибка компиляции, Other не может вызывать Foo::foo()
 *     }
 * };
 */

#ifdef restricted
#undef restricted
#endif
#define restricted public


namespace Tech {


template<typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void,
		typename T5 = void>
class PassKey {
private:
	friend T1;
	friend T2;
	friend T3;
	friend T4;
	friend T5;

	PassKey() { };
	PassKey(const PassKey&) { };
};


template<typename T1>
class PassKey<T1, void, void, void, void> {
private:
	friend T1;

	PassKey() { };
	PassKey(const PassKey&) { };
};


template<typename T1, typename T2>
class PassKey<T1, T2, void, void, void> {
private:
	friend T1;
	friend T2;

	PassKey() { };
	PassKey(const PassKey&) { };
};


template<typename T1, typename T2, typename T3>
class PassKey<T1, T2, T3, void, void> {
private:
	friend T1;
	friend T2;
	friend T3;

	PassKey() { };
	PassKey(const PassKey&) { };
};


template<typename T1, typename T2, typename T3, typename T4>
class PassKey<T1, T2, T3, T4, void> {
private:
	friend T1;
	friend T2;
	friend T3;
	friend T4;

	PassKey() { };
	PassKey(const PassKey&) { };
};


} // namespace Tech


#endif // TECH_PASSKEY_H
