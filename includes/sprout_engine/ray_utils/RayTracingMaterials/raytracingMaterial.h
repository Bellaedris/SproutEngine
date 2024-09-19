//
// Created by Bellaedris on 11/08/2024.
//

#pragma once

#include "sprout_engine/ray_utils/ray.h"
#include "sprout_engine/color.h"

#include "sprout_engine/ray_utils/traceableManager.h"

class HitInfo;

class RaytracingMaterial
{
public:
    virtual bool scatter(const Ray& p_rayIn, const HitInfo& p_hit, Color& p_color, Ray& p_rayScattered) const = 0;
};
