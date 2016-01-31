#include <gtest/gtest.h>
#include <tech/delegate.h>


using namespace Tech;


int globalInt = 0;


void f1()
{
	globalInt = 3;
}


int f2()
{
	return 5;
}


int f3(int value)
{
	return value + 1;
}


class A {
public:
	void m1()
	{
		globalInt = 18;
	}

	int m2()
	{
		return 25;
	}

	int m3(int value)
	{
		return value + 16;
	}
};


class B {
public:
	int m4() const
	{
		return 88;
	}
};


class C : public A {
public:
	int m2() const
	{
		return 44;
	}

	int m5() const
	{
		return 44;
	}
};


class D : public A, public B {
public:
	int m6() const
	{
		return 100;
	}

	virtual int m7() const
	{
		return 200;
	}
};


class E : public D {
public:
	int m7() const override
	{
		return 800;
	}
};


class VA {
public:
	int f0() const
	{
		return 0;
	}

	virtual int f1() const
	{
		return 1;
	}
};


class VB : virtual public VA {
public:
	int f2() const
	{
		return 2;
	}
};


class VC : virtual public VA {
public:
	int f3() const
	{
		return 3;
	}
};


class VD : public VB, public VC {
public:
	int f1() const override
	{
		return 11;
	}

	int f4() const
	{
		return 4;
	}
};


TEST(DelegateTest, StaticFunction)
{
	Delegate<void()> delegate0;
	ASSERT_TRUE(delegate0.isNull());

	Delegate<void()> delegate1(&f1);
	ASSERT_EQ(globalInt, 0);
	ASSERT_FALSE(delegate1.isNull());
	delegate1();
	ASSERT_EQ(globalInt, 3);
	ASSERT_NE(delegate0, delegate1);

	Delegate<int()> delegate2(&f2);
	ASSERT_FALSE(delegate2.isNull());
	ASSERT_EQ(delegate2(), 5);

	Delegate<int(int)> delegate3(&f3);
	ASSERT_FALSE(delegate3.isNull());
	ASSERT_EQ(delegate3(8), 9);

	Delegate<void()> delegate4(&f1);
	ASSERT_FALSE(delegate4.isNull());
	ASSERT_EQ(delegate4, delegate1);

	Delegate<int()> delegate5 = delegate2;
	ASSERT_FALSE(delegate5.isNull());
	ASSERT_EQ(delegate5(), 5);

	Delegate<int()> delegate6;
	ASSERT_TRUE(delegate6.isNull());
	delegate6 = delegate2;
	ASSERT_FALSE(delegate6.isNull());
	ASSERT_EQ(delegate6(), 5);

	Delegate<int()> delegate7([]() -> int { return 201; });
	ASSERT_FALSE(delegate7.isNull());
	ASSERT_EQ(delegate7(), 201);

	Delegate<int(int)> delegate8([](int value) -> int { return value - 1; });
	ASSERT_FALSE(delegate8.isNull());
	ASSERT_EQ(delegate8(115), 114);
}


TEST(DelegateTest, NoInheritanceMember)
{
	A a;
	Delegate<void()> delegate1(&a, &A::m1);
	ASSERT_EQ(globalInt, 3);
	ASSERT_FALSE(delegate1.isNull());
	delegate1();
	ASSERT_EQ(globalInt, 18);

	Delegate<int()> delegate2(&a, &A::m2);
	ASSERT_FALSE(delegate2.isNull());
	ASSERT_EQ(delegate2(), 25);

	Delegate<int(int)> delegate3(&a, &A::m3);
	ASSERT_FALSE(delegate3.isNull());
	ASSERT_EQ(delegate3(11), 27);

	B b;
	Delegate<int()> delegate4(&b, &B::m4);
	ASSERT_FALSE(delegate4.isNull());
	ASSERT_EQ(delegate4(), 88);
}


TEST(DelegateTest, SingleInheritanceMember)
{
	C c;
	Delegate<int()> delegate1(&c, &C::m2);
	ASSERT_FALSE(delegate1.isNull());
	ASSERT_EQ(delegate1(), 44);

	Delegate<int()> delegate2(&c, &A::m2);
	ASSERT_FALSE(delegate2.isNull());
	ASSERT_EQ(delegate2(), 25);
}


TEST(DelegateTest, MultipleInheritanceMember)
{
	D d;
	Delegate<int()> delegate1(&d, &D::m2);
	ASSERT_FALSE(delegate1.isNull());
	ASSERT_EQ(delegate1(), 25);

	Delegate<int()> delegate2(&d, &D::m6);
	ASSERT_FALSE(delegate2.isNull());
	ASSERT_EQ(delegate2(), 100);

	Delegate<int()> delegate3(&d, &D::m7);
	ASSERT_FALSE(delegate3.isNull());
	ASSERT_EQ(delegate3(), 200);

	E e;
	Delegate<int()> delegate4(&e, &E::m7);
	ASSERT_FALSE(delegate4.isNull());
	ASSERT_EQ(delegate4(), 800);

	delegate4 = Delegate<int()>(&e, &D::m7);
	ASSERT_FALSE(delegate4.isNull());
	ASSERT_EQ(delegate4(), 800);
}


TEST(DelegateTest, VirtualInheritanceMember)
{
	VD vd;

	Delegate<int()> delegate0(&vd, &VD::f0);
	ASSERT_FALSE(delegate0.isNull());
	ASSERT_EQ(delegate0(), 0);

	Delegate<int()> delegate1(&vd, &VD::f1);
	ASSERT_FALSE(delegate1.isNull());
	ASSERT_EQ(delegate1(), 11);

	delegate1 = Delegate<int()>(&vd, &VA::f1);
	ASSERT_FALSE(delegate1.isNull());
	ASSERT_EQ(delegate1(), 11);

	Delegate<int()> delegate2(&vd, &VD::f2);
	ASSERT_FALSE(delegate2.isNull());
	ASSERT_EQ(delegate2(), 2);

	Delegate<int()> delegate3(&vd, &VD::f3);
	ASSERT_FALSE(delegate3.isNull());
	ASSERT_EQ(delegate3(), 3);

	Delegate<int()> delegate4(&vd, &VD::f4);
	ASSERT_FALSE(delegate4.isNull());
	ASSERT_EQ(delegate4(), 4);
}
