//
// Created by Bellaedris on 11/08/2024.
//

#pragma once

#include "sprout_engine/ray_utils/ray.h"
#include "sprout_engine/color.h"

class HitInfo;

class RaytracingMaterial
{
    virtual bool scatter(const Ray& p_rayIn, const HitInfo& p_hit, const Color& p_color, Ray& p_rayScattered) const = 0;
};
