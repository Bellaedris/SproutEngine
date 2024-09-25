//
// Created by arpradier on 24/09/2024.
//

#include "TestBox.h"

#include <glm/gtc/random.hpp>
#include <sprout_engine/bounds.h>

void TestBox::SetUp()
{

}

TEST_F(TestBox, Empty)
{
    // test that an empty AABB cannot be intersected
    BoundingBox box = BoundingBox::empty();

    glm::vec3 dir;
    int hits = 0;
    for(int i = 0; i < 10000; i++)
    {
        dir = glm::sphericalRand(1.f);

        if (box.hit({glm::vec3(0, 0, 0), dir}, {0.f, std::numeric_limits<float>::max()}))
            hits++;
    }

    ASSERT_EQ(hits, 0);
}

TEST(BoundingBoxTest, RayMiss)
{
    glm::vec3 pmin(-1.0f, -1.0f, -1.0f);
    glm::vec3 pmax(1.0f, 1.0f, 1.0f);
    BoundingBox box(pmin, pmax);

    Ray ray(glm::vec3(.0f, .0f, 2.0f), glm::vec3(.0f, .0f, -.0f)); // Ray starts outside and points away from the box
    Interval<float> p_t(0.0f, 100.0f);

    EXPECT_FALSE(box.hit(ray, p_t));
}

// Ray hitting the bounding box from outside
TEST(BoundingBoxTest, RayHitOutside)
{
    glm::vec3 pmin(-1.0f, -1.0f, -1.0f);
    glm::vec3 pmax(1.0f, 1.0f, 1.0f);
    BoundingBox box(pmin, pmax);

    Ray ray(glm::vec3(0.0f, 3.0f, 0.0f), glm::vec3(0.0f, -1.f, .0f)); // Ray starts above the box and points towards it
    Interval<float> p_t(0.0f, 100.0f);

    EXPECT_TRUE(box.hit(ray, p_t));
}

// Ray starting inside the bounding box
TEST(BoundingBoxTest, RayHitInside)
{
    glm::vec3 pmin(-10.0f, -10.0f, -10.0f);
    glm::vec3 pmax(10.0f, 10.0f, 10.0f);
    BoundingBox box(pmin, pmax);

    Ray ray(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(.0f, .0f, 1.0f)); // Ray starts inside the box
    Interval<float> p_t(0.00001f, 10000.0f);

    EXPECT_TRUE(box.hit(ray, p_t));
}

// Ray parallel to one of the box's planes and missing
TEST(BoundingBoxTest, RayParallelMiss)
{
    glm::vec3 pmin(-1.0f, -1.0f, -1.0f);
    glm::vec3 pmax(1.0f, 1.0f, 1.0f);
    BoundingBox box(pmin, pmax);

    Ray ray(glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Interval<float> p_t(0.0f, 100.0f);

    EXPECT_FALSE(box.hit(ray, p_t));
}

// Ray hitting one of the box's corners
TEST(BoundingBoxTest, RayHitCorner)
{
    glm::vec3 pmin(-1.0f, -1.0f, -1.0f);
    glm::vec3 pmax(1.0f, 1.0f, 1.0f);
    BoundingBox box(pmin, pmax);

    Ray ray(glm::vec3(1.0f, 2.0f, 1.0f), glm::vec3(.0f, -1.0f, .0f));
    Interval<float> p_t(0.0f, 100.0f);

    EXPECT_TRUE(box.hit(ray, p_t));
}

// Constructing bounding box from two other boxes
TEST(BoundingBoxTest, FromTwoBoxes)
{
    glm::vec3 pmin1(-1.0f, -1.0f, -1.0f);
    glm::vec3 pmax1(0.0f, 0.0f, 0.0f);
    BoundingBox box1(pmin1, pmax1);

    glm::vec3 pmin2(0.0f, 0.0f, 0.0f);
    glm::vec3 pmax2(1.0f, 1.0f, 1.0f);
    BoundingBox box2(pmin2, pmax2);

    BoundingBox combinedBox(box1, box2);

    EXPECT_EQ(combinedBox.m_pmin, glm::vec3(-1.0f, -1.0f, -1.0f));
    EXPECT_EQ(combinedBox.m_pmax, glm::vec3(1.0f, 1.0f, 1.0f));
}

void TestBox::TearDown()
{

}
