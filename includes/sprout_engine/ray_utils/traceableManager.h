//
// Created by Bellaedris on 08/08/2024.
//

#pragma once

#include <vector>
#include <memory>

#include "sprout_engine/ray_utils/Traceables/traceable.h"
#include "sprout_engine/interval.h"
#include "sprout_engine/color.h"

/*!
 * \brief Manages a list of traceable objects and trace them
 */
class TraceableManager
{
private:
    std::vector<std::shared_ptr<Traceable>> m_traceables;

    // rendering settings
    float m_aspectRatio{1.f};
    int m_imageWidth{500};
    int m_imageHeight{500};
    glm::vec3 m_center{0, 0, 0};
    glm::vec3 m_pixelOrigin{};
    glm::vec3 m_deltaU;
    glm::vec3 m_deltaV;

    bool m_initialized{};
public:
    void setAspectRatio(float p_aspectRatio);

    void setImageWidth(int p_imageWidth);

public:
    TraceableManager() = default;
    explicit TraceableManager(std::vector<std::shared_ptr<Traceable>> p_traceables);

    void render();
    void initialize();
    Color rayColor(const Ray& p_ray);

    void add(std::shared_ptr<Traceable> p_traceable);
    void clear();

    bool hit(const Ray& p_r, Interval<float> p_t, HitInfo& p_hit) const;
};
