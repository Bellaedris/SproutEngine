#include <sprout_engine/SproutApp.h>

#include <sprout_engine/model.h>
#include <sprout_engine/texture.h>
#include <sprout_engine/line.h>
#include "sprout_engine/ray_utils/traceableManager.h"
#include <sprout_engine/ray_utils/Traceables/sphere.h>
#include <sprout_engine/interval.h>

std::string resources_path = "../../resources/";

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

        //add traceable objects
        m_traceables.setAspectRatio((float)width() / (float)height());
        m_traceables.setImageWidth(width());

        m_traceables.add(std::make_unique<Sphere>(glm::vec3(0, 0, -1), .5));
        m_traceables.add(std::make_unique<Sphere>(glm::vec3(0, -100.5, -1), 100));

        glViewport(0, 0, width(), height());
        glClearColor(.1f, .1f, .1f, 1.f);
        return 0;
    }

    int render() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Parameters");
        if (ImGui::Button("Raytrace"))
        {
            m_traceables.render();
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

            m_quad.draw(m_shader);
        }
        else
        {
            m_debugShader.use();
            m_debugShader.uniform_data("mvpMatrix", cam->projection() * cam->view());

            m_model.draw(m_debugShader);

            glm::mat4 screenToWorld = glm::inverse(viewport() * cam->projection() * cam->view());
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

protected:
    Model m_model;
    Mesh m_quad;
    Shader m_shader;
    Shader m_debugShader;
    std::unique_ptr<Texture> m_texture;

    TraceableManager m_traceables;
};

int main()
{
    Raytracer app;
    app.run();

    return 0;
}