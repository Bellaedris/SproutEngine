//
// Created by Bellaedris on 16/06/2024.
//

#ifndef SPROUTENGINE_LINE_H
#define SPROUTENGINE_LINE_H

#include <vector>
#include "color.h"
#include "shader.h"
#include "camera.h"

class LineRenderer {
protected:
    struct Line
    {
        glm::vec3 from, to;
    };

    std::vector<Line> lines;
    GLuint vao{0};
    GLuint buffer{0};
    std::unique_ptr<Shader> s;
    bool updated{};

    const ICamera* camera{};

    void createBuffers();
public:
    LineRenderer() = default;
    explicit LineRenderer(const ICamera* camera);
    ~LineRenderer();

    /*!
     * Add a new line betwen 2 points in world space
     * @param from first point of the line
     * @param to second point of the line
     * @param color color of the line
     */
    void addLine(const glm::vec3& from, const glm::vec3& to);
    void draw();
    void clear();
};


#endif //SPROUTENGINE_LINE_H
