//
// Created by Bellaedris on 08/08/2024.
//

#include <iostream>
#include "traceableManager.h"
#include "sprout_engine/image.h"

TraceableManager::TraceableManager(std::vector<std::shared_ptr<Traceable>> p_traceables)
     : m_traceables(std::move(p_traceables))
     {}



void TraceableManager::add(std::shared_ptr<Traceable> p_traceable)
{
    m_traceables.push_back(p_traceable);
}

void TraceableManager::clear()
{
    m_traceables.clear();
}

bool TraceableManager::hit(const Ray &p_r, Interval<float> p_t, HitInfo &p_hit) const
{
    HitInfo currentHit;
    bool l_hasHit{};
    float closest = p_t.m_max;

    for(const auto& l_traceable : m_traceables)
    {
        if (l_traceable->hit(p_r, {p_t.m_min, closest}, currentHit))
        {
            closest = currentHit.m_distance;
            l_hasHit = true;
        }
    }

    p_hit = currentHit;
    return l_hasHit;
}

void TraceableManager::render()
{
    if (!m_initialized)
        initialize();

    Image result(m_imageWidth, m_imageHeight);

    // for each pixel of the camera, fire a ray
    #pragma omp parallel for
    for(int i = 0; i < m_imageWidth; i++)
    {
        for(int j = 0; j < m_imageHeight; j++)
        {
            glm::vec3 currentPixel = m_pixelOrigin + (float)i * m_deltaU + (float)j * m_deltaV;

            Ray r(m_center, (currentPixel - m_center));
            result(i, j) = rayColor(r);
        }
    }

    result.write("raytrace_result.png");
}

void TraceableManager::initialize()
{
    m_imageHeight = int(m_imageWidth / m_aspectRatio);

    // m_center = {0, 0, 0} //already done in init

    float focalLength = 1.f;
    float viewportHeight = 2.0;
    float viewportWidth = viewportHeight * (float(m_imageWidth) / m_imageHeight);

    glm::vec3 viewportU(viewportWidth, 0, 0);
    glm::vec3 viewportV(0, -viewportHeight, 0);

    m_deltaU = viewportU / (float)m_imageWidth;
    m_deltaV = viewportV / (float)m_imageHeight;

    glm::vec3 viewportUpperLeft = m_center - glm::vec3(0, 0, focalLength) - viewportU / 2.f - viewportV / 2.f;
    m_pixelOrigin = viewportUpperLeft - 0.5f * (m_deltaU + m_deltaV);
}

Color TraceableManager::rayColor(const Ray &p_ray)
{
    HitInfo l_hit;
    if (hit(p_ray, {0, std::numeric_limits<float>::max()}, l_hit))
        return .5f * Color(l_hit.m_normal.x + 1.f, l_hit.m_normal.y + 1.f, l_hit.m_normal.z + 1.f, 1.f);

    // barebones skybox
    glm::vec3 unitDir = glm::normalize(p_ray.getDirection());
    float y = (unitDir.y + 1.f) * .5f;
    return lerp(Color(1.f, 1.f, 1.f, 1.f), Color(.5f, .7f, 1.f, 1.f), y);
}

void TraceableManager::setAspectRatio(float p_aspectRatio)
{
    m_aspectRatio = p_aspectRatio;
}

void TraceableManager::setImageWidth(int p_imageWidth)
{
    m_imageWidth = p_imageWidth;
}
