#include <type_traits>
#include <gtest/gtest.h>
#include <tech/types.h>


using namespace Tech;


TEST(TypesTest, Traits)
{
	// Some stupid checks for the obvious values. Just in case.
	EXPECT_EQ(sizeof(ch16), 2);
	EXPECT_EQ(sizeof(ch32), 4);
	EXPECT_EQ(sizeof(i8),   1);
	EXPECT_EQ(sizeof(u8),   1);
	EXPECT_EQ(sizeof(i16),  2);
	EXPECT_EQ(sizeof(u16),  2);
	EXPECT_EQ(sizeof(i32),  4);
	EXPECT_EQ(sizeof(u32),  4);
	EXPECT_EQ(sizeof(i64),  8);
	EXPECT_EQ(sizeof(u64),  8);
	EXPECT_EQ(sizeof(f32),  4);
	EXPECT_EQ(sizeof(f64),  8);

	EXPECT_TRUE(std::is_signed<schar>::value);
	EXPECT_TRUE(std::is_unsigned<uchar>::value);
	EXPECT_TRUE(std::is_unsigned<ushort>::value);
	EXPECT_TRUE(std::is_unsigned<uint>::value);
	EXPECT_TRUE(std::is_unsigned<ulong>::value);
	EXPECT_TRUE(std::is_unsigned<ulonglong>::value);

	EXPECT_TRUE(std::is_signed<i8>::value);
	EXPECT_TRUE(std::is_unsigned<u8>::value);
	EXPECT_TRUE(std::is_signed<i16>::value);
	EXPECT_TRUE(std::is_unsigned<u16>::value);
	EXPECT_TRUE(std::is_signed<i32>::value);
	EXPECT_TRUE(std::is_unsigned<u32>::value);
	EXPECT_TRUE(std::is_signed<i64>::value);
	EXPECT_TRUE(std::is_unsigned<u64>::value);
	EXPECT_TRUE(std::is_floating_point<f32>::value);
	EXPECT_TRUE(std::is_floating_point<f64>::value);
}


TEST(TypesTest, Box)
{
	// Is the makeBox() alias compiles?
	Box<int> box1 = makeBox<int>();
	EXPECT_EQ(*box1, 0);

	Box<int> box2 = makeBox<int>(10);
	EXPECT_EQ(*box2, 10);

	// Is the Box<> alias compiles with custom deleter specification?
	int test = 0;
	auto fakeDeleter = [](int* p) { *p = 1; };

	{
		Box<int, decltype(fakeDeleter)> box3(&test, fakeDeleter);
	}

	EXPECT_EQ(test, 1);
}


TEST(TypesTest, Arc)
{
	// Is the makeArc() alias compiles?
	Arc<int> arc1 = makeArc<int>();
	EXPECT_EQ(*arc1, 0);

	Arc<int> arc2 = makeArc<int>(8);
	EXPECT_EQ(*arc2, 8);
}


TEST(TypesTest, Pair)
{
	// Is the makePair() alias compiles?
	auto pair = makePair(15, "test");
	EXPECT_EQ(pair.first, 15);
	EXPECT_STREQ(pair.second, "test");
}


TEST(TypesTest, Tuple)
{
	// Is the makeTuple() alias compiles?
	auto tuple = makeTuple(16, "test", 25.3);
	EXPECT_EQ(std::get<0>(tuple), 16);
	EXPECT_STREQ(std::get<1>(tuple), "test");
	EXPECT_EQ(std::get<2>(tuple), 25.3);
}
