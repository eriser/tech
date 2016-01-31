#include <climits>
#include <limits>
#include <gtest/gtest.h>
#include <tech/traits.h>


using namespace Tech;


class Foo { public: void operator*() {} };
class Bar { public: };


TEST(TypeTraitsTest, Traits)
{
	EXPECT_TRUE(IsBool<bool>::value);
	EXPECT_FALSE(IsBool<int>::value);
	EXPECT_FALSE(IsBool<int*>::value);
	EXPECT_FALSE(IsBool<float>::value);
	EXPECT_FALSE(IsBool<float&>::value);
	EXPECT_FALSE(IsBool<char>::value);
	EXPECT_FALSE(IsBool<unsigned long>::value);
	EXPECT_FALSE(IsBool<Foo>::value);
	EXPECT_FALSE(IsBool<Bar>::value);

	EXPECT_FALSE(IsInteger<bool>::value);
	EXPECT_TRUE(IsInteger<int>::value);
	EXPECT_FALSE(IsInteger<int*>::value);
	EXPECT_FALSE(IsInteger<float>::value);
	EXPECT_FALSE(IsInteger<float&>::value);
	EXPECT_TRUE(IsInteger<char>::value);
	EXPECT_TRUE(IsInteger<unsigned long>::value);
	EXPECT_FALSE(IsInteger<Foo>::value);
	EXPECT_FALSE(IsInteger<Bar>::value);

	EXPECT_FALSE(IsDereferenceable<bool>::value);
	EXPECT_FALSE(IsDereferenceable<int>::value);
	EXPECT_TRUE(IsDereferenceable<int*>::value);
	EXPECT_FALSE(IsDereferenceable<float>::value);
	EXPECT_FALSE(IsDereferenceable<float&>::value);
	EXPECT_FALSE(IsDereferenceable<char>::value);
	EXPECT_FALSE(IsDereferenceable<unsigned long>::value);
	EXPECT_TRUE(IsDereferenceable<Foo>::value);
	EXPECT_FALSE(IsDereferenceable<Bar>::value);
}


TEST(TypeTraitsTest, Limits)
{
	EXPECT_TRUE(Limits<int>::bitCount() == sizeof(int) * CHAR_BIT);
	EXPECT_TRUE(Limits<int>::size() == sizeof(int));
	EXPECT_TRUE(Limits<int>::min() == std::numeric_limits<int>::min());
	EXPECT_TRUE(Limits<int>::max() == std::numeric_limits<int>::max());
	EXPECT_TRUE(Limits<int>::significantBits() == std::numeric_limits<int>::digits);

	EXPECT_TRUE(Limits<float>::bitCount() == sizeof(float) * CHAR_BIT);
	EXPECT_TRUE(Limits<float>::size() == sizeof(float));
	EXPECT_TRUE(Limits<float>::min() == std::numeric_limits<float>::lowest());
	EXPECT_TRUE(Limits<float>::max() == std::numeric_limits<float>::max());
	EXPECT_TRUE(Limits<float>::smallest() == std::numeric_limits<float>::min());
	EXPECT_TRUE(Limits<float>::mantissaSize() == std::numeric_limits<float>::digits);
	EXPECT_TRUE(Limits<float>::epsilon() == std::numeric_limits<float>::epsilon());

	EXPECT_TRUE(Limits<Foo>::bitCount() == sizeof(Foo) * CHAR_BIT);
	EXPECT_TRUE(Limits<Foo>::size() == sizeof(Foo));
}
