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
TEST(CBIterator, HandleNoneZeroInput) {
	circular_buffer<int> cb(10);
	for (int i = 1; i < 11; ++i) {
		cb.push_back(i);
	}
	int i = 1;
	for (auto it = cb.begin(); it != cb.end(); ++it) {
		EXPECT_EQ(i++,*it);
	}
	cb.clear();
	for (int i = 1; i < 16; ++i) {//6 7 8 9 10  11 12 13 14 15
		cb.push_back(i);
	}
	i = 6;
	for (auto it = cb.begin(); it != cb.end(); ++it) {
		EXPECT_EQ(i++, *it);
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
	for (int i = 0; i < 4; ++i) {
		EXPECT_EQ(i + 4, cb[0]);
		cb.pop_front();
	}
}
TEST(CBconstructTest, HandleNoneZeroInput) {
	circular_buffer<int> cb1(6);
	for (int i = 1; i < 9; ++i)	//3 4 5 6 7 8
		cb1.push_back(i);
	circular_buffer<int> cb2 = cb1;
	cb2.push_back(9);			//4 5 6 7 8 9
	int i = 3;
	for (auto it = cb1.begin(); it != cb1.end(); ++it) {
		EXPECT_EQ(i++, *it);
	}
	i = 4;
	for (auto it = cb2.begin(); it != cb2.end(); ++it) {
		EXPECT_EQ(i++, *it);
	}

	cb1 = cb2;
	i = 4;
	for (auto it = cb1.begin(); it != cb1.end(); ++it) {
		EXPECT_EQ(i++, *it);
	}
	cb1.clear();
	cb2.pop_front();
	cb1 = cb2;			//5 6 7 8 9
	i = 5;
	for (auto it = cb1.begin(); it != cb1.end(); ++it) {
		EXPECT_EQ(i++, *it);
	}
}
int _tmain(int argc, _TCHAR* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    system("pause");
    return 0;
}