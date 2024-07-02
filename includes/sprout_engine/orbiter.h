//
// Created by Bellaedris on 18/06/2024.
//

#ifndef SPROUTENGINE_ORBITER_H
#define SPROUTENGINE_ORBITER_H

#include "icamera.h"

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

class Orbiter : public ICamera
{
protected:
    glm::vec3 m_focusPoint{0, 0, 0};
    glm::vec2 m_position{0, 0};
    glm::vec2 m_rotation{0, 0};
    float m_distance{5.f};

    float m_aspectRatio{16.f / 9.f};
    float m_fov{70.f};
    float m_zNear{.1f};
    float m_zFar{1000.f};

public:
    /*!
     * \brief Look at a point from a distance
     * \param p_focusPoint point to lookt at
     * \param p_distance distance between the orbiter and the point
     */
    void lookat(const glm::vec3& p_focusPoint, float p_distance);

    /*!
     * \brief Look at the center of a bounding box
     * \param p_pmin min of the bb
     * \param p_pmax max of the bb
     */
    void lookat(const glm::vec3 &p_pmin, const glm::vec3 &p_pmax);

    /*!
     * \brief Rotate the orbiter in a sphere around the observed point
     * \param x rotation offset in X
     * \param y rotation offset in Y
     */
    void rotation(const float x, const float y);

    /*!
     * \brief changes the position of the camera
     * \param x offset x
     * \param y offset y
     */
    void translation(const float x, const float y);

    /*!
     * \brief Move the camera along the z axis, towards the observed point
     * \param z distance offset
     */
    void move(const float z);

    /*!
     * \brief returns the camera position in the world
     * \return the position of the camera in world space
     */
    glm::vec3 position() const;

    /*!
     * \brief computes and returns the projection matrix
     * \return the projection matrix
     */
    glm::mat4 projection() const;

    /*!
     * \brief Changes the aspect ratio and fov, then computes the projection matrix
     * \param p_aspectRatio new aspect ratio
     * \param p_fov new fov
     * \return the updated projection matrix
     */
    glm::mat4 projection(const float p_aspectRatio, const float p_fov);

    /*!
     * \brief Computes the view matrix
     * \return the view matrix
     */
    glm::mat4 view() const;

};


#endif //SPROUTENGINE_ORBITER_H
