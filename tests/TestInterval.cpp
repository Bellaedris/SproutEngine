//
// Created by arpradier on 30/09/2024.
//

#include "TestInterval.h"

#include <glm/gtc/random.hpp>
#include <sprout_engine/interval.h>

void TestInterval::SetUp()
{
    srand(time(0));
}

TEST_F(TestInterval, Empty) {
    Interval<float> empty = Interval<float>::empty;
    bool ret{};

    for(int i = 0; i < 100000; i++)
    {
        ret |= empty.contains(glm::linearRand(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max()));
    }

    EXPECT_FALSE(ret);
}

TEST_F(TestInterval, Universe) {
    Interval<float> universe = Interval<float>::universe;
    bool ret{};

    for(int i = 0; i < 100000; i++)
    {
        ret |= universe.contains(glm::linearRand(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max()));
    }

    EXPECT_TRUE(ret);
}

// Test for the default constructor
TEST_F(TestInterval, DefaultConstructor) {
    Interval<float> def;
    EXPECT_EQ(def.m_min, std::numeric_limits<float>::lowest());
    EXPECT_EQ(def.m_max, std::numeric_limits<float>::max());
}

// Test for the custom constructor
TEST_F(TestInterval, CustomConstructor) {
    Interval<float> interval(2.f, 8.f);
    EXPECT_EQ(interval.m_min, 2);
    EXPECT_EQ(interval.m_max, 8);
}

// Test for size()
TEST_F(TestInterval, Size) {
    Interval<float> interval(2.f, 8.f);
    EXPECT_EQ(interval.size(), 6); // 8 - 2 = 6
}

// Test for contains()
TEST_F(TestInterval, Contains) {
    Interval<float> interval(2.f, 8.f);
    EXPECT_TRUE(interval.contains(5));  // 5 is in the range [2, 8]
    EXPECT_FALSE(interval.contains(10)); // 10 is not in the range [2, 8]
}

// Test for surrounds()
TEST_F(TestInterval, Surrounds) {
    Interval<float> interval(2.f, 8.f);
    EXPECT_TRUE(interval.surrounds(5));  // 5 is in the open range (2, 8)
    EXPECT_FALSE(interval.surrounds(2)); // 2 is not in the open range (2, 8) (on boundary)
    EXPECT_FALSE(interval.surrounds(8)); // 8 is not in the open range (2, 8) (on boundary)
}

// Test for clamp()
TEST_F(TestInterval, Clamp) {
    Interval<float> interval(2.f, 8.f);
    EXPECT_EQ(interval.clamp(1), 2);   // 1 is below the min, should return 2
    EXPECT_EQ(interval.clamp(9), 8);   // 9 is above the max, should return 8
    EXPECT_EQ(interval.clamp(5), 5);   // 5 is within range, should return 5
}

void TestInterval::TearDown()
{

}