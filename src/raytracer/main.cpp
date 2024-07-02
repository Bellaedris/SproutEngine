#include <sprout_engine/SproutApp.h>

#include <sprout_engine/model.h>
#include <sprout_engine/texture.h>
#include "sprout_engine/line.h"
#include <sprout_engine/orbiter.h>

std::string resources_path = "../../resources/";

struct Hit
{
    float d;
    float u;
    float v;

    Hit() : d(std::numeric_limits<float>::max()), u(0), v(0) {}
    Hit(float d, float u, float v) : d(d), u(u), v(v) {}

    operator bool() const {
        return d != std::numeric_limits<float>::max();
    }
};

struct Ray
{
    glm::vec3 origin;
    glm::vec3 direction;
};

struct Triangle
{
    //! vertex a of the triangle
    glm::vec3 a;
    //! two edges formed by the vertex a and the other 2 vertices
    glm::vec3 edge1;
    glm::vec3 edge2;

    //! normals at the 3 vertices
    glm::vec3 na, nb, nc;

    Triangle(const vec3 &a, const vec3 &edge1, const vec3 &edge2, const vec3 &na, const vec3 &nb, const vec3 &nc)
    : a(a), edge1(edge1), edge2(edge2), na(na), nb(nb), nc(nc) {}

    /*!
     * \brief checks the intersection between this triangle and a ray,
     * from the origin to a distance htmax.
     * @param ray a ray
     * @param htmax if an intersection exists at a distance > htmax, discard it.
     * @return a struct Hit containing the infos on the hit.
     */
    Hit intersect(const Ray &ray, const float htmax) const
    {
        glm::vec3 pvec = glm::cross(ray.direction, edge2);
        float det = dot(edge1, pvec);

        float inv_det = 1.f / det;
        glm::vec3 tvec(ray.origin - a);

        float u = dot(tvec, pvec) * inv_det;
        if (u < 0 || u > 1)
            return {};

        glm::vec3 qvec = cross(tvec, edge1);
        float v = dot(ray.direction, qvec) * inv_det;
        if (v < 0 || u + v > 1)
            return {};

        float t = dot(edge2, qvec) * inv_det;
        if (t < 0 || t > htmax)
            return {};

        return {t, u, v};
    }
};

class Raytracer : public SproutApp
{
public:
    Raytracer() : SproutApp(1366, 768, 4, 6) {}

    int init() override
    {
        //read a mesh and create a camera
        m_model = Model(resources_path + "models/cornell-box.obj");
        cam = new Camera({0, 2, 5}, {0, 1, 0}, 0, -90.f, 0.1f, 100.f, 70.f, (float)width() / (float)height());
        m_quad = Mesh::generatePlane();
        m_shader = Shader("texture.vs", "texture.fs");
        m_debugShader = Shader("default.vs", "default.fs");

        m_camera.lookat({0, 2, 0}, 15.f);
        m_camera.rotation(0, 180);
        m_camera.projection((float)width() / (float)height(), 70.f);

        m_lines = std::make_unique<LineRenderer>(cam);

        glViewport(0, 0, width(), height());
        glClearColor(.1f, .1f, .1f, 1.f);
        return 0;
    }

    int render() override
    {
        // deplace la camera
        if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
            m_camera.rotation(xoffset * delta_time * 5.f, yoffset * delta_time * 5.f);      // tourne autour de l'objet
        if (glfwGetKey(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
            m_camera.translation((float) xoffset / (float) width(), (float) yoffset / (float) height()); // deplace le point de rotation
        else if (glfwGetKey(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
            m_camera.move(yoffset);           // approche / eloigne l'objet

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Parameters");
        if (ImGui::Button("Raytrace"))
        {
            raytrace();
        }
        if (ImGui::Button("Raster"))
        {
            m_texture.release();
        }
        ImGui::End();

        if (m_texture)
        {
            //render the image
            m_texture->use();
            m_shader.use();

            m_quad.draw_strip(m_shader);
        }
        else
        {
            m_debugShader.use();
            m_debugShader.uniform_data("mvpMatrix", cam->projection() * cam->view());

            m_model.draw(m_debugShader);

            glm::mat4 screenToWorld = glm::inverse(viewport() * cam->projection() * cam->view());

            m_lines->clear();
            m_lines->addLine({screenToWorld * glm::vec4(0, 0, 0, 1)}, {screenToWorld * glm::vec4(0, 0, 1, 1)});
            m_lines->addLine({screenToWorld * glm::vec4(width(), 0, 0, 1)}, {screenToWorld * glm::vec4(width(), 0, 1, 1)});
            m_lines->addLine({screenToWorld * glm::vec4(0, height(), 0, 1)}, {screenToWorld * glm::vec4(0, height(), 1, 1)});
            m_lines->addLine({screenToWorld * glm::vec4(width(), height(), 0, 1)}, {screenToWorld * glm::vec4(width(), height(), 1, 1)});

            m_lines->draw();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        return 0;
    }

    int quit() override
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        return 0;
    }

    void raytrace()
    {
        std::vector<Triangle> triangles;

        //build the triangle structure
        for(const Mesh& mesh : m_model.getMeshes())
            for(size_t triId = 0; triId + 2 < mesh.indices().size(); triId += 3)
            {
                int ia = mesh.indice(triId);

                glm::vec3 a = mesh.position(ia);
                glm::vec3 b = mesh.position(mesh.indice(triId + 1));
                glm::vec3 c = mesh.position(mesh.indice(triId + 2));

                //TODO don't forget to inverse transpose the normal on GPU
                glm::vec3 na = mesh.normal(mesh.indice(triId));
                glm::vec3 nb = mesh.normal(mesh.indice(triId + 1));
                glm::vec3 nc = mesh.normal(mesh.indice(triId + 2));

                triangles.emplace_back(a, glm::vec3(b - a), glm::vec3(c - a), na, nb, nc);
            }

        Image result(width(), height());

        glm::mat4 screenToWorld = glm::inverse(viewport() * cam->projection() * cam->view());

        // for each pixel of the camera, fire a ray
        #pragma omp parallel for
        for(int i = 0; i < width(); i++)
        {
            for(int j = 0; j < height(); j++)
            {
                glm::vec4 origin = screenToWorld * glm::vec4(i, j, 0, 1);
                glm::vec4 direction = (screenToWorld * glm::vec4(i, j, 1, 1)) - origin;
                Ray r{origin, direction};

                Hit closest;

                // iterate over all triangles of the mesh to render. If it hits, we can return the color of the pixel
                for(const Triangle& triangle : triangles)
                {
                    if (Hit h = triangle.intersect(r, closest.d))
                    {
                        if (h.d < closest.d)
                            closest = h;
                    }
                }

                if (closest)
                {
                    //get barycentric coord
                    float w = 1 - closest.u - closest.v;

                    result(i, j) = Color(w, closest.u, closest.v, 1.f);
                }
            }
        }

        result.write("raytrace_result.png");


    }

protected:
    Model m_model;
    Mesh m_quad;
    Shader m_shader;
    Shader m_debugShader;
    std::unique_ptr<Texture> m_texture;
    Orbiter m_camera;

    std::unique_ptr<LineRenderer> m_lines;
};

int main()
{
    Raytracer app;
    app.run();

    return 0;
}