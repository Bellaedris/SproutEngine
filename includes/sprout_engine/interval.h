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

    Interval() : m_min(std::numeric_limits<T>::lowest()), m_max(std::numeric_limits<T>::max()) {}

    Interval(T p_min, T p_max) : m_min(p_min), m_max(p_max) {}

    /*!
     * \brief computes the size of the interval: max - min
     * \return the size of the interval
     */
    T size() const
    {
        return (m_max - m_min);
    }

    /*!
     * \brief Checks if p_x is inside the closed interval
     * \param p_x number to check
     * \return true if p_x is inside the closed interval, false otherwise
     */
    bool contains(T p_x) const
    {
        return p_x >= m_min && p_x <= m_max;
    }

    /*!
     * \brief Checks if p_x is inside the open interval
     * \param p_x number to check
     * \return true if p_x is in the open interval, false otherwise
     */
    bool surrounds(T p_x) const
    {
        return p_x > m_min && p_x < m_max;
    }

    /*!
     * \brief Clamps a value in the interval
     * \param p_x value to clamp
     * \return min of the interval if p_x is below, max if it is above, p_x otherwise
     */
    T clamp(T p_x) const
    {
        if (p_x < m_min)
            return m_min;
        if (p_x > m_max)
            return m_max;
        return p_x;
    }

    static const Interval empty, universe;
};

template<typename T>
const Interval<T> Interval<T>::empty = {std::numeric_limits<T>::infinity(), -std::numeric_limits<T>::infinity()};

template<typename T>
const Interval<T> Interval<T>::universe = {-std::numeric_limits<T>::infinity(), std::numeric_limits<T>::infinity()};