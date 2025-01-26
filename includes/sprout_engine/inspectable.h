//
// Created by Bellaedris on 28/04/2024.
//

#pragma once

#include <IMGUI/imgui.h>

class Inspectable {
    virtual void drawInspector() = 0;
};
