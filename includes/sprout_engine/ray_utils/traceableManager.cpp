//
// Created by Bellaedris on 08/08/2024.
//

#include <iostream>


#include "traceableManager.h"
#include "sprout_engine/image.h"

#include <glm/gtc/random.hpp>

#include "RayTracingMaterials/raytracingMaterial.h"

TraceableManager::TraceableManager(std::vector<std::shared_ptr<Traceable>> p_traceables, Camera* p_camera)
     : m_traceables(std::move(p_traceables))
     , m_camera(p_camera)
     , m_distribution(0.f, 1.f)
     , m_generator(std::random_device{}())
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

glm::vec4 TraceableManager::generateRayOnHemisphere()
{
    return {glm::sphericalRand(1.f), 1.f};
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
            Color pixelColor{0.f, 0.f, 0.f ,0.f};
            for(int sample = 0; sample < m_samplesPerPixel; sample++)
            {
                pixelColor += rayColor(generateRayInPixel(i, j), m_maxBounces);
            }
            result(i, j) = pixelColor / (float)m_samplesPerPixel;
        }
    }

    result.write("raytrace_result.png");
}

void TraceableManager::initialize()
{
    m_imageHeight = int(m_imageWidth / m_aspectRatio);

    float focalLength = glm::length(m_camera->get_position() - m_camera->getDir());
    float h = std::tanf(glm::radians(m_camera->getFov()) / 2.f);
    float viewportHeight = 2.f * h * focalLength;
    float viewportWidth = viewportHeight * m_camera->getAspectRatio();

    glm::vec3 viewportU = viewportWidth * m_camera->getRight();
    glm::vec3 viewportV = -viewportHeight * m_camera->getUp();

    m_deltaU = viewportU / (float)m_imageWidth;
    m_deltaV = viewportV / (float)m_imageHeight;

    glm::vec3 viewportUpperLeft = m_center - focalLength * -m_camera->getDir() - viewportU / 2.f - viewportV / 2.f;
    m_pixelOrigin = viewportUpperLeft - 0.5f * (m_deltaU + m_deltaV);
}

Color TraceableManager::rayColor(const Ray &p_ray, int p_depth)
{
    if(p_depth <= 0)
        return {0.f, 0.f, 0.f, 1.f};

    HitInfo l_hit;
    if (hit(p_ray, {0.0001f, (std::numeric_limits<float>::max)()}, l_hit))
    {
        // bounces the ray randomly
        // this is lambertian non-uniform, get more rays in the normal direction by just adding a random vector to the norm
        Ray scattered;
        Color attenuation;
        if (l_hit.m_mat->scatter(p_ray, l_hit, attenuation, scattered))
            return attenuation * rayColor(scattered, p_depth - 1);
        //glm::vec3 l_bounceDir = l_hit.m_normal + glm::sphericalRand(1.f); //generateRayOnHemisphere(l_hit.m_normal);
        // uniform distribution: the rays are equally distributed
        //glm::vec3 l_bounceDir = generateRayOnHemisphere(l_hit.m_normal);
        //TODO implement the cookbook method 35 iirc
        //return .5f * Color(l_hit.m_normal.x + 1.f, l_hit.m_normal.y + 1.f, l_hit.m_normal.z + 1.f, 1.f);
        //return .5f * rayColor({l_hit.m_intersection, l_bounceDir}, p_depth - 1);
        return Black();
    }

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

void TraceableManager::setSamplesPerPixel(int p_samplesPerPixel)
{

}

void TraceableManager::setCamera(Camera* p_camera)
{
    m_camera = p_camera;
}

Ray TraceableManager::generateRayInPixel(int x, int y)
{
    glm::vec3 offset{getRayOffset() - .5f, getRayOffset() - .5f, 0.f};
    glm::vec3 currentSample = m_pixelOrigin + ((float)x + offset.x) * m_deltaU + ((float)y + offset.y) * m_deltaV;

    return{m_center, (currentSample - m_center)};
}

float TraceableManager::getRayOffset()
{
    return m_distribution(m_generator);
}

glm::vec3 TraceableManager::generateRayOnHemisphere(const glm::vec3& p_normal)
{
    glm::vec3 res = glm::sphericalRand(1.f);
    if(glm::dot(res, p_normal) > 0.f)
        return res;
    else
        return -res;
}
