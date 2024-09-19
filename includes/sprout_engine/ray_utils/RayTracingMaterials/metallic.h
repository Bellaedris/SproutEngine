//
// Created by arpradier on 17/09/2024.
//


#pragma once

#include "raytracingMaterial.h"

/**
 * \brief a metallic material that reflects the light almost perfectly
 */
class Metallic : public RaytracingMaterial
{
public:
    explicit Metallic(const Color& albedo, float p_fuzz) : m_albedo(albedo), m_fuzz(p_fuzz) {}

    bool scatter(const Ray& p_rayIn, const HitInfo& p_hit, Color& p_color, Ray& p_rayScattered) const override;

private:
    //! Color
    Color m_albedo;

    //! Small perturbation added to the scattered ray
    float m_fuzz;
};
