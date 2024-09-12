//
// Created by arpradier on 10/09/2024.
//


#include "TestImage.h"

#include <random>

void TestImage::SetUp()
{
    m_image = Image(600, 500);
}

TEST_F(TestImage, EmptyImage) {
    for(int j = 0; j < 500; j++)
        for(int i = 0; i < 600; i++)
            m_image(i, j) = Black();

    m_image.write("tests/blackImage.png");
}

TEST_F(TestImage, WhiteImage) {
    for(int j = 0; j < 500; j++)
        for(int i = 0; i < 600; i++)
            m_image(i, j) = White();

    m_image.write("tests/whiteImage.png");
}

TEST_F(TestImage, PinkImage) {
    for(int j = 0; j < 500; j++)
        for(int i = 0; i < 600; i++)
            m_image(i, j) = Color(1.f, 0.f, 1.f, 1.f);

    m_image.write("tests/pinkImage.png");
}

TEST_F(TestImage, FrenchFlag) {
    Color toWrite;
    for(int j = 0; j < 500; j++)
        for(int i = 0; i < 600; i++)
        {
            if (i < 200)
                toWrite = Blue();
            else if (i < 400)
                toWrite = White();
            else
                toWrite = Red();
            m_image(i, j) = toWrite;
        }

    m_image.write("tests/frenchFlag.png");
}

TEST_F(TestImage, Circles) {
    std::random_device r;

    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> rand_width(0, 500);
    std::uniform_int_distribution<int> rand_size(5, 30);
    std::uniform_real_distribution<float> rand_col(0.f, 1.f);

    int numberOfSpheres = 15;
    std::vector<Sphere> spheres;

    for (int i = 0; i < numberOfSpheres; i++)
    {
        Color c(rand_col(e1), rand_col(e1), rand_col(e1), 1.f);

        spheres.emplace_back(
            glm::vec2(rand_width(e1), rand_width(e1)),
            rand_size(e1),
            c
        );
    }

    for(int j = 0; j < 500; j++)
        for(int i = 0; i < 600; i++)
        {
            bool hit{};
            for(const auto& sphere : spheres)
                if (sphere.PointInSphere(i, j))
                {
                    m_image(i, j) = sphere.m_color;
                    hit = true;
                }
            if (!hit)
                m_image(i, j) = White();
        }

    m_image.write("tests/circles.png");
}

TEST_F(TestImage, Square) {
    std::random_device r;

    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> rand_width(0, 500);
    std::uniform_int_distribution<int> rand_size(5, 30);
    std::uniform_real_distribution<float> rand_col(0.f, 1.f);

    int numberOfSquares = 15;
    std::vector<Square> squares;

    for (int i = 0; i < numberOfSquares; i++)
    {
        Color c(rand_col(e1), rand_col(e1), rand_col(e1), 1.f);

        squares.emplace_back(
            glm::vec2(rand_width(e1), rand_width(e1)),
            rand_size(e1),
            c
        );
    }

    for(int j = 0; j < 500; j++)
        for(int i = 0; i < 600; i++)
        {
            bool hit{};
            for(const auto& square : squares)
                if (square.PointInSquare(i, j))
                {
                    m_image(i, j) = square.m_color;
                    hit = true;
                }
            if (!hit)
                m_image(i, j) = White();
        }

    m_image.write("tests/square.png");
}

TEST_F(TestImage, Gradient) {
    Color start = Black();
    Color end = Blue();

    for(int j = 0; j < 500; j++)
        for(int i = 0; i < 600; i++)
            m_image(i, j) = lerp(start, end, (float)j / 500.f);

    m_image.write("tests/gradient.png");
}

TEST_F(TestImage, ChristmasTree) {

    glm::vec2 begin(300, 60);
    int size = 30;
    int stacks = 7;

    std::vector<Square> squares;

    for(int i = 1; i < stacks; i++)
    {
        squares.emplace_back(begin + glm::vec2(0.f, size * 2 * i), size + 1, Color(0.1647f, .1843f, .1373f, 1.f));
        for(int j = 0; j < i; j++)
        {
            squares.emplace_back(begin + glm::vec2(size * j, size * 2 * i), size + 1, Color(0.1647f, .1843f, .1373f, 1.f));
            squares.emplace_back(begin + glm::vec2(-size * j, size * 2 * i), size + 1, Color(0.1647f, .1843f, .1373f, 1.f));
        }
    }
    squares.emplace_back(begin + glm::vec2(0.f , size * 2 * stacks - 1), size, Color(0.2314f, .2039f, .1608f, 1.f));


    for(int j = 0; j < 500; j++)
        for(int i = 0; i < 600; i++)
        {
            bool hit{};
            for(const auto& square : squares)
                if (square.PointInSquare(i, j))
                {
                    m_image(i, j) = square.m_color;
                    hit = true;
                }
            if (!hit)
                m_image(i, j) = Black();
        }

    m_image.write("tests/ChristmasTree.png");
}

TEST_F(TestImage, Mandelbrot) {
    Color start = Black();
    Color end = Blue();

    for(int j = 0; j < 500; j++)
        for(int i = 0; i < 600; i++)
            m_image(i, j) = lerp(start, end, (float)j / 500.f);

    m_image.write("tests/gradient.png");
}


void TestImage::TearDown()
{
    //delete m_image;
}