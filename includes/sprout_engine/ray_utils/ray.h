//
// Created by Bellaedris on 22/07/2024.
//

#ifndef SPROUTENGINE_RAY_H
#define SPROUTENGINE_RAY_H

#include <glm/glm.hpp>

class Ray {
protected:
    glm::vec3 m_origin{};
    glm::vec3 m_direction{};

public:
    Ray() = default;
    Ray(const glm::vec3& p_origin, const glm::vec3& p_direction);

    [[nodiscard]] glm::vec3 getOrigin() const {return m_origin;}
    [[nodiscard]] glm::vec3 getDirection() const {return m_direction;}

    glm::vec3 at(float t) const;
};


#endif //SPROUTENGINE_RAY_H
