#include <gtest/gtest.h>
#include <tech/utils.h>


using namespace Tech;


TEST(UtilsTest, Minimum)
{
	ASSERT_EQ(minOf(10, 20), 10);
	ASSERT_EQ(minOf(20, 10), 10);
	ASSERT_EQ(minOf(10, 10), 10);
	ASSERT_EQ(minOf(-10, 20), -10);
	ASSERT_EQ(minOf(10, -20), -20);
	ASSERT_EQ(minOf(-10, -10), -10);
	ASSERT_EQ(minOf(0, 0), 0);
	ASSERT_EQ(minOf(45, -15, 0, 64), -15);
}


TEST(UtilsTest, Maximum)
{
	ASSERT_EQ(maxOf(10, 20), 20);
	ASSERT_EQ(maxOf(20, 10), 20);
	ASSERT_EQ(maxOf(10, 10), 10);
	ASSERT_EQ(maxOf(-10, 20), 20);
	ASSERT_EQ(maxOf(10, -20), 10);
	ASSERT_EQ(maxOf(-10, -10), -10);
	ASSERT_EQ(maxOf(0, 0), 0);
	ASSERT_EQ(maxOf(45, -15, 0, 64), 64);
}


TEST(UtilsTest, Bound)
{
	ASSERT_EQ(bound(10, 20, 30), 20);
	ASSERT_EQ(bound(10, 40, 30), 30);
	ASSERT_EQ(bound(10, 5, 30), 10);
	ASSERT_EQ(bound(10, -5, 30), 10);
	ASSERT_EQ(bound(10, 10, 30), 10);
	ASSERT_EQ(bound(10, 30, 30), 30);
	ASSERT_EQ(bound(10, 10, 10), 10);
	ASSERT_EQ(bound(-30, 5, -10), -10);
	ASSERT_EQ(bound(-30, -5, -10), -10);
	ASSERT_EQ(bound(-30, -15, -10), -15);
	ASSERT_EQ(bound(-30, -35, -10), -30);
}


TEST(UtilsTest, Whole)
{
	ASSERT_EQ(whole(10.2f), 10);
	ASSERT_EQ(whole(10.2), 10);
	ASSERT_EQ(whole(16.5f), 16);
	ASSERT_EQ(whole(16.5), 16);
	ASSERT_EQ(whole(-156.93485f), -156);
	ASSERT_EQ(whole(-156.93485), -156);
	ASSERT_EQ(whole(35.0), 35);
}


TEST(UtilsTest, Fraction)
{
//	ASSERT_TRUE(fuzzyIsEqual(fraction(10.2f), 0.2f));
//	ASSERT_TRUE(fuzzyIsEqual(fraction(10.2), 0.2));
//	ASSERT_TRUE(fuzzyIsEqual(fraction(-156.93485f), -0.93485f));
//	ASSERT_TRUE(fuzzyIsEqual(fraction(-156.93485), -0.93485));
}


TEST(UtilsTest, Round)
{
	ASSERT_EQ(round(10.2f), 10.0f);
	ASSERT_EQ(round(10.2), 10.0);
	ASSERT_EQ(round(10.5f), 11.0f);
	ASSERT_EQ(round(10.5), 11.0);
	ASSERT_EQ(round(10.8f), 11.0f);
	ASSERT_EQ(round(10.8), 11.0);
	ASSERT_EQ(round(10.0f), 10.0f);
	ASSERT_EQ(round(10.0), 10.0);
	ASSERT_EQ(round(-10.2f), -10.0f);
	ASSERT_EQ(round(-10.2), -10.0);
	ASSERT_EQ(round(-10.5f), -11.0f);
	ASSERT_EQ(round(-10.5), -11.0);
	ASSERT_EQ(round(-10.8f), -11.0f);
	ASSERT_EQ(round(-10.8), -11.0);
	ASSERT_EQ(round(-10.0f), -10.0f);
	ASSERT_EQ(round(-10.0), -10.0);
}


TEST(UtilsTest, CountLeadingZeroes)
{
	u32 value32 = 0x00000000;
	u64 value64 = 0x0000000000000000;
	ASSERT_EQ(countLeadingZeroes(value32), 32);
	ASSERT_EQ(countLeadingZeroesHelper32(value32), 32);
	ASSERT_EQ(countLeadingZeroes(value64), 64);
	ASSERT_EQ(countLeadingZeroesHelper64(value64), 64);

	value32 = 0x00000001;
	value64 = 0x0000000000000001;

	ASSERT_EQ(countLeadingZeroes(value32), 31);
	ASSERT_EQ(countLeadingZeroesHelper32(value32), 31);
	ASSERT_EQ(countLeadingZeroes(value64), 63);
	ASSERT_EQ(countLeadingZeroesHelper64(value64), 63);

	value32 = 0x80000001;
	value64 = 0x8000000000000000;
	ASSERT_EQ(countLeadingZeroes(value32), 0);
	ASSERT_EQ(countLeadingZeroesHelper32(value32), 0);
	ASSERT_EQ(countLeadingZeroes(value64), 0);
	ASSERT_EQ(countLeadingZeroesHelper64(value64), 0);

	value32 = 0x80000001;
	value64 = 0x8000000000000001;
	ASSERT_EQ(countLeadingZeroes(value32), 0);
	ASSERT_EQ(countLeadingZeroesHelper32(value32), 0);
	ASSERT_EQ(countLeadingZeroes(value64), 0);
	ASSERT_EQ(countLeadingZeroesHelper64(value64), 0);

	value32 = 0x00010000;
	value64 = 0x0000000100000000;
	ASSERT_EQ(countLeadingZeroes(value32), 15);
	ASSERT_EQ(countLeadingZeroesHelper32(value32), 15);
	ASSERT_EQ(countLeadingZeroes(value64), 31);
	ASSERT_EQ(countLeadingZeroesHelper64(value64), 31);

	value32 = 0xFFFFFFFF;
	value64 = 0xFFFFFFFFFFFFFFFF;
	ASSERT_EQ(countLeadingZeroes(value32), 0);
	ASSERT_EQ(countLeadingZeroesHelper32(value32), 0);
	ASSERT_EQ(countLeadingZeroes(value64), 0);
	ASSERT_EQ(countLeadingZeroesHelper64(value64), 0);
}


TEST(UtilsTest, MostSignificantBit)
{
	ASSERT_EQ(mostSignificantBit(0x00000000), -1);
	ASSERT_EQ(mostSignificantBit(0x000104F0), 16);
}


TEST(UtilsTest, CountSetBits)
{
	u32 value32 = 0x00000000;
	u64 value64 = 0x0000000000000000;
	ASSERT_EQ(countSetBits(value32), 0);
	ASSERT_EQ(countSetBitsHelper32(value32), 0);
	ASSERT_EQ(countSetBits(value64), 0);
	ASSERT_EQ(countSetBitsHelper64(value64), 0);

	value32 = 0x00000001;
	value64 = 0x0000000100000000;
	ASSERT_EQ(countSetBits(value32), 1);
	ASSERT_EQ(countSetBitsHelper32(value32), 1);
	ASSERT_EQ(countSetBits(value64), 1);
	ASSERT_EQ(countSetBitsHelper64(value64), 1);

	value32 = 0x80000000;
	value64 = 0x8000000000000000;
	ASSERT_EQ(countSetBits(value32), 1);
	ASSERT_EQ(countSetBitsHelper32(value32), 1);
	ASSERT_EQ(countSetBits(value64), 1);
	ASSERT_EQ(countSetBitsHelper64(value64), 1);

	value32 = 0x00001000;
	value64 = 0x0000000000001000;
	ASSERT_EQ(countSetBits(value32), 1);
	ASSERT_EQ(countSetBitsHelper32(value32), 1);
	ASSERT_EQ(countSetBits(value64), 1);
	ASSERT_EQ(countSetBitsHelper64(value64), 1);

	value32 = 0x80000001;
	value64 = 0x8000000000000001;
	ASSERT_EQ(countSetBits(value32), 2);
	ASSERT_EQ(countSetBitsHelper32(value32), 2);
	ASSERT_EQ(countSetBits(value64), 2);
	ASSERT_EQ(countSetBitsHelper64(value64), 2);

	value32 = 0x00800100;
	value64 = 0x0080000000000100;
	ASSERT_EQ(countSetBits(value32), 2);
	ASSERT_EQ(countSetBitsHelper32(value32), 2);
	ASSERT_EQ(countSetBits(value64), 2);
	ASSERT_EQ(countSetBitsHelper64(value64), 2);

	value32 = 0x00000007;
	value64 = 0x0000000000000007;
	ASSERT_EQ(countSetBits(value32), 3);
	ASSERT_EQ(countSetBitsHelper32(value32), 3);
	ASSERT_EQ(countSetBits(value64), 3);
	ASSERT_EQ(countSetBitsHelper64(value64), 3);

	value32 = 0xE0000000;
	value64 = 0xE000000000000000;
	ASSERT_EQ(countSetBits(value32), 3);
	ASSERT_EQ(countSetBitsHelper32(value32), 3);
	ASSERT_EQ(countSetBits(value64), 3);
	ASSERT_EQ(countSetBitsHelper64(value64), 3);

	value32 = 0x021F0701;
	value64 = 0x021F070100000000;
	ASSERT_EQ(countSetBits(value32), 10);
	ASSERT_EQ(countSetBitsHelper32(value32), 10);
	ASSERT_EQ(countSetBits(value64), 10);
	ASSERT_EQ(countSetBitsHelper64(value64), 10);

	value32 = 0xFFFFFFFF;
	value64 = 0xFFFFFFFFFFFFFFFF;
	ASSERT_EQ(countSetBits(value32), 32);
	ASSERT_EQ(countSetBitsHelper32(value32), 32);
	ASSERT_EQ(countSetBits(value64), 64);
	ASSERT_EQ(countSetBitsHelper64(value64), 64);
}


TEST(UtilsTest, CountResetBits)
{
	u32 value = 0x00000000;
	ASSERT_EQ(countResetBits(value), 32);

	value = 0x00000001;
	ASSERT_EQ(countResetBits(value), 31);

	value = 0x80000000;
	ASSERT_EQ(countResetBits(value), 31);

	value = 0x00001000;
	ASSERT_EQ(countResetBits(value), 31);

	value = 0x80000001;
	ASSERT_EQ(countResetBits(value), 30);

	value = 0x00800100;
	ASSERT_EQ(countResetBits(value), 30);

	value = 0x00000007;
	ASSERT_EQ(countResetBits(value), 29);

	value = 0xE0000000;
	ASSERT_EQ(countResetBits(value), 29);

	value = 0x021F0701;
	ASSERT_EQ(countResetBits(value), 22);

	value = 0xFFFFFFFF;
	ASSERT_EQ(countResetBits(value), 0);
}


TEST(UtilsTest, RotateBitsLeft)
{
	u32 value = 0x80000000;
	ASSERT_EQ(rotateBitsLeft(value, 1), 0x00000001);
	ASSERT_EQ(rotateBitsLeft(value, 33), 0x00000001);

	value = 0x01000000;
	ASSERT_EQ(rotateBitsLeft(value, 10), 0x00000004);
}


TEST(UtilsTest, RotateBitsRight)
{
	u32 value = 0x00000001;
	ASSERT_EQ(rotateBitsRight(value, 1), 0x80000000);
	ASSERT_EQ(rotateBitsRight(value, 33), 0x80000000);

	value = 0x00000080;
	ASSERT_EQ(rotateBitsRight(value, 10), 0x20000000);
}


TEST(UtilsTest, RotateBits)
{
	u32 value = 0x80000001;
	ASSERT_EQ(rotateBits(value, 1), 0xC0000000);
	ASSERT_EQ(rotateBits(value, -1), 0x00000003);
}


TEST(UtilsTest, MostSignificantDigit)
{
	u32 value = 34785604;
	ASSERT_EQ(mostSignificantDigit(value), 7);

	value = 0;
	ASSERT_EQ(mostSignificantDigit(value), -1);
}


TEST(UtilsTest, ReverseBits)
{
	u32 value = 0x8B150000;
	ASSERT_EQ(reverseBits(value), 0x0000A8D1);

	value = 0;
	ASSERT_EQ(reverseBits(value), 0);
}


TEST(UtilsTest, SwapBytes)
{
	ASSERT_EQ(swapBytesHelper16(0x0102), 0x0201);
	ASSERT_EQ(swapBytes(static_cast<u16>(0x0102)), 0x0201);

	ASSERT_EQ(swapBytesHelper32(0x01020304), 0x04030201);
	ASSERT_EQ(swapBytes(0x01020304), 0x04030201);

	ASSERT_EQ(swapBytesHelper64(0x0102030405060708), 0x0807060504030201);
	ASSERT_EQ(swapBytes(0x0102030405060708), 0x0807060504030201);
}


TEST(UtilsTest, HighBits)
{
	ASSERT_EQ(highBits(0x01033040, 16), 0x0103);
	ASSERT_EQ(highBits(0x01033040, 15), 0x81);
	ASSERT_EQ(highBits(0x01033040, 0), 0);
	ASSERT_EQ(highBits(0x01033040, -2), 0);
}


TEST(UtilsTest, LowBits)
{
	ASSERT_EQ(lowBits(0x01033040, 16), 0x3040);
	ASSERT_EQ(lowBits(0x0103B040, 15), 0x3040);
	ASSERT_EQ(lowBits(0x01033040, 0), 0);
	ASSERT_EQ(lowBits(0x01033040, -2), 0);
}


TEST(UtilsTest, IsPowerOfTwo)
{
	ASSERT_FALSE(isPowerOfTwo(0));
	ASSERT_FALSE(isPowerOfTwo(0x349234));
	ASSERT_TRUE(isPowerOfTwo(1));
	ASSERT_TRUE(isPowerOfTwo(0x100));
	ASSERT_TRUE(isPowerOfTwo(0x100000));
}


TEST(UtilsTest, CeilToPowerOfTwo)
{
	ASSERT_EQ(ceilToPowerOfTwo(0x100), 0x100);
	ASSERT_EQ(ceilToPowerOfTwo(0xFD), 0x100);
	ASSERT_EQ(ceilToPowerOfTwo(1), 1);
	ASSERT_EQ(ceilToPowerOfTwo(0), 1);
}


TEST(UtilsTest, FloorToPowerOfTwo)
{
	ASSERT_EQ(floorToPowerOfTwo(0x100), 0x100);
	ASSERT_EQ(floorToPowerOfTwo(0x18F), 0x100);
	ASSERT_EQ(floorToPowerOfTwo(1), 1);
	ASSERT_EQ(floorToPowerOfTwo(3), 2);
	ASSERT_EQ(floorToPowerOfTwo(0), 0);
}
