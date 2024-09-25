//
// Created by arpradier on 12/09/2024.
//

#include "utils.h"

#include <random>

float Utils::random01()
{
    std::uniform_real_distribution<float> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

float Utils::randomRange(float p_min, float p_max)
{
    std::random_device rd;
    std::uniform_real_distribution<float> distribution(p_min, p_max);
    static std::mt19937 generator(rd());
    return distribution(generator);
}
