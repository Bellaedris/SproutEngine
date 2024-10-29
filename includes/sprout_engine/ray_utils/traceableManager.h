//
// Created by Bellaedris on 08/08/2024.
//

#pragma once

#include <vector>
#include <memory>
#include <random>
#include <sprout_engine/camera.h>

#include "sprout_engine/ray_utils/Traceables/traceable.h"
#include "sprout_engine/interval.h"
#include "sprout_engine/color.h"

/*!
 * \brief Manages a list of traceable objects and trace them
 */
class TraceableManager
{
protected:
    std::vector<Traceable*> m_traceables;

    // rendering settings
    float m_aspectRatio{1.f};
    int m_imageWidth{500};
    int m_imageHeight{500};
    glm::vec3 m_center{0, 0, 0};
    glm::vec3 m_pixelOrigin{};
    glm::vec3 m_deltaU{};
    glm::vec3 m_deltaV{};

    int m_samplesPerPixel{1};
    int m_maxBounces{5};

    //! Camera that gives informations about what the scene views
    Camera* m_camera;

    BoundingBox m_boundingBox;

    bool m_initialized{};

    //random numbers
    std::uniform_real_distribution<float> m_distribution{};
    std::mt19937 m_generator;
    std::random_device m_randDevice;

    Color rayColor(const Ray &p_ray, int p_depth);

    /*!
     * \brief Generates a ray inside the pixel x, y
     * \param x x coordinates in the image
     * \param y y coordinates in the image
     * \return A ray that points towards a random location inside the pixel
     */
    Ray generateRayInPixel(int x, int y);

    /*!
     * \brief Generates a ray on the hemisphere centered on the normal
     * \param p_normal normal of the surface
     * \return a unit ray on the hemisphere
     */
    glm::vec3 generateRayOnHemisphere(const glm::vec3& p_normal);

    /*!
     * \brief Generates a random number between 0 and 1, used to get a random offset in a pixel
     * \return a float between 0 and 1
     */
    float getRayOffset();
public:
    TraceableManager() = default;
    explicit TraceableManager(std::vector<Traceable*> p_traceables, Camera* p_camera);

    //! \brief Sends rays to actually trace the scene
    void render();
    void drawBoundingBoxes(Shader& s, int maxDepth);
    //! \brief initializes all the parameters of the ray tracer. Always done before first draw.
    void initialize();

    /*!
     * \brief Add a Traceable to the scene
     * \param p_traceable a traceable object
     */
    void add(Traceable* p_traceable);

    /*!
     * \brief Add an array of Traceables to the scene
     * \param p_traceable an array of pointers to traceables
     */
    void add(const std::vector<Traceable*>& p_traceables);

    //! \brief clears the scene
    void clear();

    /*!
     * \brief Shoots a ray through the scene and keeps the closest hit
     * \param p_r a ray
     * \param p_t Interval in which our rays can intersect
     * \param p_hit informations on the hit
     * \return true if the ray hits something, false otherwise
     */
    bool hit(const Ray& p_r, Interval<float> p_t, HitInfo& p_hit) const;

    /*!
    * \brief Generates a ray on the hemisphere centered on the normal
    * \return a unit ray on the hemisphere
    */
    static glm::vec4 generateRayOnHemisphere();

    void setAspectRatio(float p_aspectRatio);
    void setImageWidth(int p_imageWidth);
    void setSamplesPerPixel(int p_samplesPerPixel);
    void setMaxBounces(int p_maxBounces);
    void setCamera(Camera* p_camera);

    /**
     * gets the traceables pointers
     * @return A copy of the traceables
     */
    std::vector<Traceable*> getTraceables();
};
