//
// Created by Bellaedris on 18/06/2024.
//

#ifndef SPROUTENGINE_ICAMERA_H
#define SPROUTENGINE_ICAMERA_H

#include "glm/glm.hpp"

class ICamera
{
public:
    [[nodiscard]] virtual glm::mat4 view() const = 0;
    [[nodiscard]] virtual glm::mat4 projection() const = 0;
};

#endif //SPROUTENGINE_ICAMERA_H
