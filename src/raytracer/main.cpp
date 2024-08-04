#include <sprout_engine/SproutApp.h>

#include <sprout_engine/model.h>
#include <sprout_engine/texture.h>
#include "sprout_engine/line.h"
#include <sprout_engine/orbiter.h>
#include <sprout_engine/ray_utils/ray.h>

Color rayColor(const Ray& p_ray)
{
    float y = (p_ray.getDirection().y + 1.f) * .5f;
    return lerp(Color(1.f, 1.f, 1.f, 1.f), Color(.5f, .7f, 1.f, 1.f), y);
}

float intersectSphere(const glm::vec3 p_center, float radius, const Ray& ray)
{
    glm::vec3 a = 
}

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

    void raytrace()
    {
        Image result(width(), height());

        auto focalLength = 1.0;
        auto viewportHeight = 2.0;
        auto viewportWidth = viewportHeight * (double(width()) / height());
        auto cameraCenter = glm::vec3(0, 0, 0);

        glm::vec3 viewportU(width(), 0, 0);
        glm::vec3 viewportV(0, -height(), 0);

        glm::vec3 deltaU = viewportU / (float)width();
        glm::vec3 deltaV = viewportV / (float)height();

        glm::vec3 viewportUpperLeft = cameraCenter - glm::vec3(0, 0, focalLength) - viewportU / 2.f - viewportV / 2.f;
        glm::vec3 pixelOrigin = viewportUpperLeft - 0.5f * (deltaU + deltaV);

        // for each pixel of the camera, fire a ray
        #pragma omp parallel for
        for(int i = 0; i < width(); i++)
        {
            for(int j = 0; j < height(); j++)
            {
                glm::vec3 currentPixel = pixelOrigin + (float)i * deltaU + (float)j * deltaV;

                Ray r(cameraCenter, (currentPixel - cameraCenter));
                result(i, j) = rayColor(r);
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
};

int main()
{
    Raytracer app;
    app.run();

    return 0;
}