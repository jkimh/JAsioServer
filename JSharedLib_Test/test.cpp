#include "pch.h"
#include "Vector3.h"
using namespace JSharedLib;
TEST(Vector3, Vector3OperatorTest) 
{
	Vector3 a(1, 2, 3);
	Vector3 b(2, 1, 0);

	Vector3 c = a + b;
	EXPECT_EQ(c.x, 3);
	EXPECT_EQ(c.y, 3);
	EXPECT_EQ(c.z, 3);

	c = a - b;
	EXPECT_EQ(c.x, -1);
	EXPECT_EQ(c.y, 1);
	EXPECT_EQ(c.z, 3);

	EXPECT_EQ(a*b, 4);

	Vector3 d(1, 2, 3);
	EXPECT_TRUE(a == d); 
	EXPECT_FALSE(a == b);

	c = a;
	EXPECT_EQ(c.x, 1);
	EXPECT_EQ(c.y, 2);
	EXPECT_EQ(c.z, 3);
}