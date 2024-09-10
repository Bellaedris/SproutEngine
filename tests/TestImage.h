//
// Created by arpradier on 10/09/2024.
//

#pragma once

#include <gtest/gtest.h>

#include <sprout_engine/image.h>

class TestImage : public testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;

    Image m_image{};

    struct Sphere
    {
        glm::vec2 center;
        int size;
        Color m_color;

        Sphere(const glm::vec2& center, int size, const Color& color) :
            center(center), size(size), m_color(color) {}

        bool PointInSphere(int x, int y) const
        {
            return (glm::distance(center, glm::vec2{ x, y }) < size);
        }
    };

    struct Square
    {
        glm::vec2 center;
        int size;
        Color m_color;

        Square(const glm::vec2& center, int size, const Color& color) :
            center(center), size(size), m_color(color) {}

        bool PointInSquare(int x, int y) const
        {
            return (std::abs(x - static_cast<int>(center.x)) < size && std::abs(y - static_cast<int>(center.y)) < size);
        }
    };
};
