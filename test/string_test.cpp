#include <gtest/gtest.h>
#include <tech/binary.h>
#include <tech/string.h>


using namespace Tech;


bool isEqual(const String& string, const ch16* data)
{
	const ch16* str = reinterpret_cast<const ch16*>(string.constData());
	size_t count = string.length();
	size_t size = std::char_traits<ch16>::length(data);

	if(count != size)
		return false;

	while(count--) {
		if(*str != *data)
			return false;

		++str;
		++data;
	}

	return true;
}


TEST(StringTest, DefaultConstruction)
{
	String string;

	ASSERT_TRUE(string.isNull());
	ASSERT_TRUE(string.isEmpty());
	ASSERT_EQ(string.length(), 0);

	String clone = string;

	ASSERT_TRUE(clone.isNull());
	ASSERT_TRUE(clone.isEmpty());
	ASSERT_EQ(clone.length(), 0);
}


TEST(StringTest, ConstructionFromCString)
{
	String string("This is a test");

	ASSERT_FALSE(string.isNull());
	ASSERT_FALSE(string.isEmpty());
	ASSERT_EQ(string.length(), 14);
	ASSERT_TRUE(isEqual(string, u"This is a test"));
}


TEST(StringTest, ConstructionFromCStringWithSize)
{
	String string("This is a test", 4);

	ASSERT_FALSE(string.isNull());
	ASSERT_FALSE(string.isEmpty());
	ASSERT_EQ(string.length(), 4);
	ASSERT_TRUE(isEqual(string, u"This"));
}


TEST(StringTest, ConstructionFromU16String)
{
	String string(u"This is a test");

	ASSERT_FALSE(string.isNull());
	ASSERT_FALSE(string.isEmpty());
	ASSERT_EQ(string.length(), 14);
	ASSERT_TRUE(isEqual(string, u"This is a test"));
}


TEST(StringTest, ConstructionFromU16StringWithSize)
{
	String string(u"This is a test", 4);

	ASSERT_FALSE(string.isNull());
	ASSERT_FALSE(string.isEmpty());
	ASSERT_EQ(string.length(), 4);
	ASSERT_TRUE(isEqual(string, u"This"));
}


TEST(StringTest, ConstructionFromByteArray)
{
	Binary ba("This is a test");
	String string(ba);

	ASSERT_FALSE(string.isNull());
	ASSERT_FALSE(string.isEmpty());
	ASSERT_EQ(string.length(), 14);
	ASSERT_TRUE(isEqual(string, u"This is a test"));
}


TEST(StringTest, ConstructionFromString)
{
	String other("This is a test");
	String string(other);

	ASSERT_FALSE(string.isNull());
	ASSERT_FALSE(string.isEmpty());
	ASSERT_EQ(string.length(), 14);
	ASSERT_TRUE(isEqual(string, u"This is a test"));
}


TEST(StringTest, ConstructionFromChar)
{
	Char ch = 'x';
	String string(ch);

	ASSERT_FALSE(string.isNull());
	ASSERT_FALSE(string.isEmpty());
	ASSERT_EQ(string.length(), 1);
	ASSERT_TRUE(isEqual(string, u"x"));
}


TEST(StringTest, ConstructionFromSharedCopy)
{
	String string("This is a test");
	String clone = string;

	ASSERT_FALSE(string.isNull());
	ASSERT_FALSE(string.isEmpty());
	ASSERT_EQ(string.length(), 14);
	ASSERT_TRUE(isEqual(string, u"This is a test"));

	ASSERT_FALSE(clone.isNull());
	ASSERT_FALSE(clone.isEmpty());
	ASSERT_EQ(clone.length(), 14);
	ASSERT_TRUE(isEqual(clone, u"This is a test"));
}


TEST(StringTest, Clear)
{
	String string = "This is a test";
	string.clear();
	ASSERT_FALSE(string.isNull());
	ASSERT_TRUE(string.isEmpty());

	string = "This is a test";
	String clone = string;

	string.clear();

	ASSERT_FALSE(string.isNull());
	ASSERT_TRUE(string.isEmpty());

	ASSERT_FALSE(clone.isNull());
	ASSERT_FALSE(clone.isEmpty());

	clone.clear();

	ASSERT_FALSE(clone.isNull());
	ASSERT_TRUE(clone.isEmpty());
}


TEST(StringTest, Swap)
{
	String string1 = "string 1";
	String string2 = "string 2";

	string1.swap(string2);

	ASSERT_TRUE(isEqual(string1, u"string 2"));
	ASSERT_TRUE(isEqual(string2, u"string 1"));

	String clone = string1;
	string1.swap(string2);

	ASSERT_TRUE(isEqual(string1, u"string 1"));
	ASSERT_TRUE(isEqual(string2, u"string 2"));
	ASSERT_TRUE(isEqual(clone, u"string 2"));
}


TEST(StringTest, Left)
{
	String string = "This is a test";

	String result1 = string.left(4);
	ASSERT_TRUE(isEqual(string, u"This is a test"));
	ASSERT_TRUE(isEqual(result1, u"This"));

	String clone = string;

	String result2 = string.left(4);
	ASSERT_TRUE(isEqual(string, u"This is a test"));
	ASSERT_TRUE(isEqual(clone, u"This is a test"));
	ASSERT_TRUE(isEqual(result2, u"This"));

	String result3 = string.left(0);
	ASSERT_TRUE(isEqual(result3, u""));

	String result4 = string.left(100);
	ASSERT_TRUE(isEqual(result4, u"This is a test"));
}


TEST(StringTest, Middle)
{
	String string = "This is a test";

	String result1 = string.middle(5);
	ASSERT_TRUE(isEqual(string, u"This is a test"));
	ASSERT_TRUE(isEqual(result1, u"is a test"));

	String clone = string;

	String result2 = string.middle(5, 2);
	ASSERT_TRUE(isEqual(string, u"This is a test"));
	ASSERT_TRUE(isEqual(clone, u"This is a test"));
	ASSERT_TRUE(isEqual(result2, u"is"));

	String result3 = string.middle(5, 100);
	ASSERT_TRUE(isEqual(result3, u"is a test"));
}


TEST(StringTest, Right)
{
	String string = "This is a test";

	String result1 = string.right(4);
	ASSERT_TRUE(isEqual(string, u"This is a test"));
	ASSERT_TRUE(isEqual(result1, u"test"));

	String clone = string;

	String result2 = string.right(4);
	ASSERT_TRUE(isEqual(string, u"This is a test"));
	ASSERT_TRUE(isEqual(clone, u"This is a test"));
	ASSERT_TRUE(isEqual(result2, u"test"));

	String result3 = string.right(100);
	ASSERT_TRUE(isEqual(result3, u"This is a test"));
}


TEST(StringTest, LeftJustified)
{
	String string = "test";

	String result1 = string.leftJustified(2, '-', true);
	ASSERT_TRUE(isEqual(string, u"test"));
	ASSERT_TRUE(isEqual(result1, u"te"));

	String clone = string;

	String result2 = string.leftJustified(8, '-');
	ASSERT_TRUE(isEqual(string, u"test"));
	ASSERT_TRUE(isEqual(clone, u"test"));
	ASSERT_TRUE(isEqual(result2, u"test----"));

	String result3 = string.leftJustified(8, '-', true);
	String result4 = string.leftJustified(2, '-', false);
	ASSERT_TRUE(isEqual(result3, u"test----"));
	ASSERT_TRUE(isEqual(result4, u"test"));
}


TEST(StringTest, MiddleJustified)
{
	String string = "test";

	String result1 = string.middleJustified(2, '-', true);
	ASSERT_TRUE(isEqual(string, u"test"));
	ASSERT_TRUE(isEqual(result1, u"es"));

	String clone = string;

	String result2 = string.middleJustified(8, '-');
	ASSERT_TRUE(isEqual(string, u"test"));
	ASSERT_TRUE(isEqual(clone, u"test"));
	ASSERT_TRUE(isEqual(result2, u"--test--"));

	String result3 = string.middleJustified(8, '-', true);
	String result4 = string.middleJustified(2, '-', false);
	ASSERT_TRUE(isEqual(result3, u"--test--"));
	ASSERT_TRUE(isEqual(result4, u"test"));
}


TEST(StringTest, RightJustified)
{
	String string = "test";

	String result1 = string.rightJustified(2, '-', true);
	ASSERT_TRUE(isEqual(string, u"test"));
	ASSERT_TRUE(isEqual(result1, u"st"));

	String clone = string;

	String result2 = string.rightJustified(8, '-');
	ASSERT_TRUE(isEqual(string, u"test"));
	ASSERT_TRUE(isEqual(clone, u"test"));
	ASSERT_TRUE(isEqual(result2, u"----test"));

	String result3 = string.rightJustified(8, '-', true);
	String result4 = string.rightJustified(2, '-', false);
	ASSERT_TRUE(isEqual(result3, u"----test"));
	ASSERT_TRUE(isEqual(result4, u"test"));
}


TEST(StringTest, Fill)
{
	String string = "test";

	string.fill('-');
	ASSERT_TRUE(isEqual(string, u"----"));

	String clone = string;
	string.fill('+');

	ASSERT_TRUE(isEqual(string, u"++++"));
	ASSERT_TRUE(isEqual(clone, u"----"));
}


TEST(StringTest, StartsWith)
{
	String string = "This is a test";

	ASSERT_TRUE(string.startsWith('T'));
	ASSERT_TRUE(string.startsWith("This"));
}


TEST(StringTest, EndsWith)
{
	String string = "This is a test";

	ASSERT_TRUE(string.endsWith('t'));
	ASSERT_TRUE(string.endsWith("test"));
}


TEST(StringTest, SplitChar)
{
	String string = "This is  a test";
	StringList fields = string.split(' ', String::kKeepEmptyParts);

	ASSERT_TRUE(fields.size() == 5);
	ASSERT_TRUE(fields[0] == "This");
	ASSERT_TRUE(fields[1] == "is");
	ASSERT_TRUE(fields[2] == "");
	ASSERT_TRUE(fields[3] == "a");
	ASSERT_TRUE(fields[4] == "test");

	fields = string.split(' ', String::kSkipEmptyParts);

	ASSERT_TRUE(fields.size() == 4);
	ASSERT_TRUE(fields[0] == "This");
	ASSERT_TRUE(fields[1] == "is");
	ASSERT_TRUE(fields[2] == "a");
	ASSERT_TRUE(fields[3] == "test");
}


TEST(StringTest, SplitString)
{
	String string = "Thisis a test";
	StringList fields = string.split("is", String::kKeepEmptyParts);

	ASSERT_TRUE(fields.size() == 3);
	ASSERT_TRUE(isEqual(fields[0], u"Th"));
	ASSERT_TRUE(isEqual(fields[1], u""));
	ASSERT_TRUE(isEqual(fields[2], u" a test"));

	fields = string.split("is", String::kSkipEmptyParts);

	ASSERT_TRUE(fields.size() == 2);
	ASSERT_TRUE(isEqual(fields[0], u"Th"));
	ASSERT_TRUE(isEqual(fields[1], u" a test"));
}


TEST(StringTest, Chop)
{
	String string = "This is a test";

	string.chop(5);
	ASSERT_TRUE(isEqual(string, u"This is a"));

	String clone = string;
	string.chop(5);

	ASSERT_TRUE(isEqual(string, u"This"));
	ASSERT_TRUE(isEqual(clone, u"This is a"));

	string.chop(100);
	ASSERT_TRUE(string.isEmpty());
}


TEST(StringTest, Truncate)
{
	String string = "This is a test";

	string.truncate(9);
	ASSERT_TRUE(isEqual(string, u"This is a"));

	String clone = string;
	string.truncate(4);

	ASSERT_TRUE(isEqual(string, u"This"));
	ASSERT_TRUE(isEqual(clone, u"This is a"));

	string.truncate(100);
	ASSERT_TRUE(isEqual(string, u"This"));
}

/*
TEST(StringTest, Simplified)
{
	String string = " \t  This is a\t test  ";

	String result = string.simplified();
	ASSERT_TRUE(stringEqual(result, u"This is a test"));
}


TEST(StringTest, Trimmed)
{
	String string = " \t  This is a test  ";

	String result = string.trimmed();
	ASSERT_TRUE(stringEqual(result, u"This is a test"));
}
*/


TEST(StringTest, Append)
{
	String string = "This is";
	string.append(" a ");

	ASSERT_TRUE(isEqual(string, u"This is a "));

	String clone = string;

	string.append("test");
	ASSERT_TRUE(isEqual(string, u"This is a test"));
	ASSERT_TRUE(isEqual(clone, u"This is a "));
}


TEST(StringTest, Prepend)
{
	String string = "a test";
	string.prepend(" is ");

	ASSERT_TRUE(isEqual(string, u" is a test"));

	String clone = string;

	string.prepend("This");
	ASSERT_TRUE(isEqual(string, u"This is a test"));
	ASSERT_TRUE(isEqual(clone, u" is a test"));
}


TEST(StringTest, Insert)
{
	String string = " a ";
	string.insert(String::kNoPos, "test");

	ASSERT_TRUE(isEqual(string, u" a test"));

	String clone = string;

	string.insert(0, "This");
	ASSERT_TRUE(isEqual(string, u"This a test"));
	ASSERT_TRUE(isEqual(clone, u" a test"));

	string.insert(4, " is");
	ASSERT_TRUE(isEqual(string, u"This is a test"));
	ASSERT_TRUE(isEqual(clone, u" a test"));
}


TEST(StringTest, RemoveCount)
{
	String string = "This is a test";
	string.remove(4, 3);

	ASSERT_TRUE(isEqual(string, u"This a test"));

	String clone = string;

	string.remove(0, 5);
	ASSERT_TRUE(isEqual(string, u"a test"));
	ASSERT_TRUE(isEqual(clone, u"This a test"));

	string.remove(1, 5);
	ASSERT_TRUE(isEqual(string, u"a"));
	ASSERT_TRUE(isEqual(clone, u"This a test"));

	string.remove(100, 1);
	ASSERT_TRUE(isEqual(string, u"a"));
	ASSERT_TRUE(isEqual(clone, u"This a test"));

	string.remove(0, 0);
	ASSERT_TRUE(isEqual(string, u"a"));
	ASSERT_TRUE(isEqual(clone, u"This a test"));

	string.remove(0, 100);
	ASSERT_TRUE(string.isEmpty());
	ASSERT_TRUE(isEqual(clone, u"This a test"));
}


TEST(StringTest, Remove)
{
	String string = "This is a test";
	string.remove("is");

	ASSERT_TRUE(isEqual(string, u"Th  a test"));

	String clone = string;

	string.remove("t");
	ASSERT_TRUE(isEqual(string, u"Th  a es"));
	ASSERT_TRUE(isEqual(clone, u"Th  a test"));
}


TEST(StringTest, Replace)
{
	String string = "This is a test";
	string.replace("is", "1$");

	ASSERT_TRUE(isEqual(string, u"Th1$ 1$ a test"));

	String clone = string;

	string.replace("1$", "1234");
	ASSERT_TRUE(isEqual(string, u"Th1234 1234 a test"));
	ASSERT_TRUE(isEqual(clone, u"Th1$ 1$ a test"));

	string.replace("1234", "");
	ASSERT_TRUE(isEqual(string, u"Th  a test"));
	ASSERT_TRUE(isEqual(clone, u"Th1$ 1$ a test"));

	string.replace(2, 4, "--------");
	ASSERT_TRUE(isEqual(string, u"Th--------test"));
}


TEST(StringTest, IndexOf)
{
	String string = "This is a test";
	ASSERT_EQ(string.indexOf('s', 0), 3);
	ASSERT_EQ(string.indexOf("is", 0), 2);
	ASSERT_EQ(string.indexOf("is", 2), 2);
	ASSERT_EQ(string.indexOf("is", 3), 5);
	ASSERT_EQ(string.indexOf("is", 7), String::kNoPos);
	ASSERT_EQ(string.indexOf("is", 100), String::kNoPos);

	ASSERT_EQ(string.lastIndexOf("is", String::kNoPos), 5);
	ASSERT_EQ(string.lastIndexOf("is", 6), 5);
	ASSERT_EQ(string.lastIndexOf("is", 5), 2);
	ASSERT_EQ(string.lastIndexOf("is", 1), String::kNoPos);

	String longString = "But I must explain to you how all this mistaken "
			"idea of denouncing pleasure and praising pain was born and I will "
			"give you a complete account of the system, and expound the actual "
			"teachings of the great explorer of the truth, the master-builder "
			"of human happiness. No one rejects, dislikes, or avoids pleasure "
			"itself, because it is pleasure, but because those who do not know "
			"how to pursue pleasure rationally encounter consequences that are "
			"extremely painful. Nor again is there anyone who loves or pursues "
			"or desires to obtain pain of itself, because it is pain, but "
			"because occasionally circumstances occur in which toil and pain "
			"can procure him some great pleasure. To take a trivial example, "
			"which of us ever undertakes laborious physical exercise, except to"
			" obtain some advantage from it? But who has any right to find "
			"fault with a man who chooses to enjoy a pleasure that has no "
			"annoying consequences, or one who avoids a pain that produces no "
			"resultant pleasure?";

	ASSERT_EQ(longString.indexOf("pleasure", 0), 67);
	ASSERT_EQ(longString.indexOf("pleasure", 67), 67);
	ASSERT_EQ(longString.indexOf("pleasure", 68), 301);
	ASSERT_EQ(longString.indexOf("pleasure", 302), 332);
	ASSERT_EQ(longString.indexOf("pleasure", 333), 390);
	ASSERT_EQ(longString.indexOf("pleasure", 391), 660);
	ASSERT_EQ(longString.indexOf("pleasure", 661), 865);
	ASSERT_EQ(longString.indexOf("pleasure", 866), 961);
	ASSERT_EQ(longString.indexOf("pleasure", 962), String::kNoPos);

	ASSERT_EQ(longString.lastIndexOf("pleasure", String::kNoPos), 961);
	ASSERT_EQ(longString.lastIndexOf("pleasure", 961), 961);
	ASSERT_EQ(longString.lastIndexOf("pleasure", 960), 865);
	ASSERT_EQ(longString.lastIndexOf("pleasure", 864), 660);
	ASSERT_EQ(longString.lastIndexOf("pleasure", 659), 390);
	ASSERT_EQ(longString.lastIndexOf("pleasure", 389), 332);
	ASSERT_EQ(longString.lastIndexOf("pleasure", 331), 301);
	ASSERT_EQ(longString.lastIndexOf("pleasure", 300), 67);
	ASSERT_EQ(longString.lastIndexOf("pleasure", 66), String::kNoPos);
}

