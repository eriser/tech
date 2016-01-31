#include <gtest/gtest.h>
#include <tech/bytearray.h>


using namespace Tech;


TEST(ByteArrayTest, DefaultConstruction)
{
	ByteArray ba;

	ASSERT_TRUE(ba.isNull());
	ASSERT_TRUE(ba.isEmpty());
	ASSERT_EQ(ba.length(), 0);

	ByteArray clone = ba;

	ASSERT_TRUE(clone.isNull());
	ASSERT_TRUE(clone.isEmpty());
	ASSERT_EQ(clone.length(), 0);
}


TEST(ByteArrayTest, ConstructionFromCString)
{
	ByteArray ba("This is a test");

	ASSERT_FALSE(ba.isNull());
	ASSERT_FALSE(ba.isEmpty());
	ASSERT_EQ(ba.length(), 14);
	ASSERT_STRCASEEQ(ba, "This is a test");
}


TEST(ByteArrayTest, ConstructionFromCStringWithSize)
{
	ByteArray ba("This is a test", 4);

	ASSERT_FALSE(ba.isNull());
	ASSERT_FALSE(ba.isEmpty());
	ASSERT_EQ(ba.length(), 4);
	ASSERT_STRCASEEQ(ba, "This");
}


TEST(ByteArrayTest, ConstructionFromByteArray)
{
	ByteArray other("This is a test");
	ByteArray ba(other);

	ASSERT_FALSE(ba.isNull());
	ASSERT_FALSE(ba.isEmpty());
	ASSERT_EQ(ba.length(), 14);
	ASSERT_STRCASEEQ(ba, "This is a test");
}


TEST(ByteArrayTest, ConstructionFromChar)
{
	ByteArray ba('x');

	ASSERT_FALSE(ba.isNull());
	ASSERT_FALSE(ba.isEmpty());
	ASSERT_EQ(ba.length(), 1);
	ASSERT_STRCASEEQ(ba, "x");
}


TEST(ByteArrayTest, ConstructionFromSharedCopy)
{
	ByteArray ba("This is a test");
	ByteArray clone = ba;

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
	ByteArray ba = "This is a test";
	ba.clear();
	ASSERT_FALSE(ba.isNull());
	ASSERT_TRUE(ba.isEmpty());

	ba = "This is a test";
	ByteArray clone = ba;

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
	ByteArray ba1 = "string 1";
	ByteArray ba2 = "string 2";

	ba1.swap(ba2);

	ASSERT_STRCASEEQ(ba1, "string 2");
	ASSERT_STRCASEEQ(ba2, "string 1");

	ByteArray clone = ba1;
	ba1.swap(ba2);

	ASSERT_STRCASEEQ(ba1, "string 1");
	ASSERT_STRCASEEQ(ba2, "string 2");
	ASSERT_STRCASEEQ(clone, "string 2");
}


TEST(ByteArrayTest, Left)
{
	ByteArray ba = "This is a test";

	ByteArray result1 = ba.left(4);
	ASSERT_STRCASEEQ(ba, "This is a test");
	ASSERT_STRCASEEQ(result1, "This");

	ByteArray clone = ba;

	ByteArray result2 = ba.left(4);
	ASSERT_STRCASEEQ(ba, "This is a test");
	ASSERT_STRCASEEQ(clone, "This is a test");
	ASSERT_STRCASEEQ(result2, "This");

	ByteArray result3 = ba.left(0);
	ASSERT_STRCASEEQ(result3, "");

	ByteArray result4 = ba.left(100);
	ASSERT_STRCASEEQ(result4, "This is a test");
}
