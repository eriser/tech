#include <gtest/gtest.h>
#include <tech/binary.h>


using namespace Tech;


TEST(ByteArrayTest, DefaultConstruction)
{
	Binary ba;

	ASSERT_TRUE(ba.isNull());
	ASSERT_TRUE(ba.isEmpty());
	ASSERT_EQ(ba.length(), 0);

	Binary clone = ba;

	ASSERT_TRUE(clone.isNull());
	ASSERT_TRUE(clone.isEmpty());
	ASSERT_EQ(clone.length(), 0);
}


TEST(ByteArrayTest, ConstructionFromCString)
{
	Binary ba("This is a test");

	ASSERT_FALSE(ba.isNull());
	ASSERT_FALSE(ba.isEmpty());
	ASSERT_EQ(ba.length(), 14);
	ASSERT_STRCASEEQ(ba, "This is a test");
}


TEST(ByteArrayTest, ConstructionFromCStringWithSize)
{
	Binary ba("This is a test", 4);

	ASSERT_FALSE(ba.isNull());
	ASSERT_FALSE(ba.isEmpty());
	ASSERT_EQ(ba.length(), 4);
	ASSERT_STRCASEEQ(ba, "This");
}


TEST(ByteArrayTest, ConstructionFromByteArray)
{
	Binary other("This is a test");
	Binary ba(other);

	ASSERT_FALSE(ba.isNull());
	ASSERT_FALSE(ba.isEmpty());
	ASSERT_EQ(ba.length(), 14);
	ASSERT_STRCASEEQ(ba, "This is a test");
}


TEST(ByteArrayTest, ConstructionFromChar)
{
	Binary ba('x');

	ASSERT_FALSE(ba.isNull());
	ASSERT_FALSE(ba.isEmpty());
	ASSERT_EQ(ba.length(), 1);
	ASSERT_STRCASEEQ(ba, "x");
}


TEST(ByteArrayTest, ConstructionFromSharedCopy)
{
	Binary ba("This is a test");
	Binary clone = ba;

	ASSERT_FALSE(ba.isNull());
	ASSERT_FALSE(ba.isEmpty());
	ASSERT_EQ(ba.length(), 14);
	ASSERT_STRCASEEQ(ba, "This is a test");

	ASSERT_FALSE(clone.isNull());
	ASSERT_FALSE(clone.isEmpty());
	ASSERT_EQ(clone.length(), 14);
	ASSERT_STRCASEEQ(clone, "This is a test");
}


TEST(ByteArrayTest, Clear)
{
	Binary ba = "This is a test";
	ba.clear();
	ASSERT_FALSE(ba.isNull());
	ASSERT_TRUE(ba.isEmpty());

	ba = "This is a test";
	Binary clone = ba;

	ba.clear();

	ASSERT_FALSE(ba.isNull());
	ASSERT_TRUE(ba.isEmpty());

	ASSERT_FALSE(clone.isNull());
	ASSERT_FALSE(clone.isEmpty());

	clone.clear();

	ASSERT_FALSE(clone.isNull());
	ASSERT_TRUE(clone.isEmpty());
}


TEST(ByteArrayTest, Swap)
{
	Binary ba1 = "string 1";
	Binary ba2 = "string 2";

	ba1.swap(ba2);

	ASSERT_STRCASEEQ(ba1, "string 2");
	ASSERT_STRCASEEQ(ba2, "string 1");

	Binary clone = ba1;
	ba1.swap(ba2);

	ASSERT_STRCASEEQ(ba1, "string 1");
	ASSERT_STRCASEEQ(ba2, "string 2");
	ASSERT_STRCASEEQ(clone, "string 2");
}


TEST(ByteArrayTest, Left)
{
	Binary ba = "This is a test";

	Binary result1 = ba.left(4);
	ASSERT_STRCASEEQ(ba, "This is a test");
	ASSERT_STRCASEEQ(result1, "This");

	Binary clone = ba;

	Binary result2 = ba.left(4);
	ASSERT_STRCASEEQ(ba, "This is a test");
	ASSERT_STRCASEEQ(clone, "This is a test");
	ASSERT_STRCASEEQ(result2, "This");

	Binary result3 = ba.left(0);
	ASSERT_STRCASEEQ(result3, "");

	Binary result4 = ba.left(100);
	ASSERT_STRCASEEQ(result4, "This is a test");
}
