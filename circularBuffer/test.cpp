#include <tchar.h>
#include <gtest/gtest.h>
#include "circularBuffer.h"


TEST(CBasicTest, HandleNoneZeroInput)
{
	circular_buffer<int> cb(3);
	EXPECT_EQ(true, cb.empty());
	EXPECT_EQ(0, cb.size());

	cb.push_back(1);
	EXPECT_EQ(1, cb.top());
	cb.pop_front();
	for (int i = 0; i < 3; ++i) {// 1 2 3
		cb.push_back(i);
		EXPECT_EQ(i + 1, cb.size());
	}
	for (int i = 0; i < 3; ++i) {
		EXPECT_EQ(i, cb.top());
		cb.pop_front();
	}
	EXPECT_EQ(0, cb.size());
	for (int i = 1; i < 5; ++i) {// 2 3 4
		cb.push_back(i);
	}
	for (int i = 2; i < 5; ++i) {
		EXPECT_EQ(i, cb.top());
		cb.pop_front();
	}
}
TEST(CBIndexTest, HandleNoneZeroInput) {
	circular_buffer<int> cb(4);
	for (int i = 1; i < 5; ++i) {//1 2 3 4 
		cb.push_back(i);
	}
	for (int i = 0; i < 4; ++i) {
		EXPECT_EQ(i + 1, cb[i]);
	}
	for (int i = 5; i < 8; ++i) {//5 6 7 4
		cb.push_back(i);
	}
	for (int i = 0; i < 4; ++i) {
		EXPECT_EQ(i + 4, cb[i]);
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    system("pause");
    return 0;
}