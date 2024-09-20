//
// Created by arpradier on 12/09/2024.
//

#include "utils.h"

#include <random>

float Utils::random01()
{
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}
