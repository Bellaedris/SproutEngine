//
// Created by Bellaedris on 08/08/2024.
//

#pragma once

#include <limits>
#include <type_traits>

template<typename T>
class Interval
{
public:
    T m_min, m_max;

    //! enforce arithmetic type. Could be improved with cpp 23 concepts, but stb image is stuck in 17 for now
    static_assert(std::is_arithmetic<T>::value, "Template parameter T must be of arithmetic type");

    Interval() : m_min(std::numeric_limits<T>::min()), m_max(std::numeric_limits<T>::max()) {}

    Interval(T p_min, T p_max) : m_min(p_min), m_max(p_max) {}

    T size() const
    {
        return (m_max - m_min);
    }

    bool contains(T p_x) const
    {
        return p_x >= m_min && p_x <= m_max;
    }

    bool surrounds(T p_x) const
    {
        return p_x > m_min && p_x < m_max;
    }

    static const Interval empty, universe;
};

template<typename T>
const Interval<T> Interval<T>::empty = {std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity()};

template<typename T>
const Interval<T> Interval<T>::universe = {-std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity()};