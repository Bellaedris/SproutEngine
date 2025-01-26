//
// Created by Bellaedris on 28/04/2024.
//

#pragma once

#include <IMGUI/imgui.h>
#include <IMGUI/ImGuizmo.h>
#include "camera.h"

class Inspectable {
    virtual void drawInspector(Camera *camera) = 0;
};
