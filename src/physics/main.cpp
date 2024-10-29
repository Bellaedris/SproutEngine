//
// Created by arpradier on 14/10/2024.
//

#include <thread>
#include <sprout_engine/entity.h>
#include <sprout_engine/light.h>
#include <sprout_engine/line.h>
#include <sprout_engine/SproutApp.h>
#include <sprout_engine/physics/physicsSolver.h>
#include <sprout_engine/physics/springJoint.h>

std::string resources_path = "../../resources/";

class PhysicsApp : public SproutApp
{
public:
    PhysicsApp() : SproutApp(1366, 768, 4, 6), spring(nullptr) {}

    int init() override
    {
        simulatedSpheres.emplace_back(new PhysicsEntity(resources_path + "models/sphere.obj", "Sphere1"));
        simulatedSpheres.emplace_back(new PhysicsEntity(resources_path + "models/sphere.obj", "Sphere2"));
        camera = Camera({0, 0, 10}, {0, 1, 0}, 0.f, -90.f, 0.01f, 1000.f, 80.f, (float)m_width / (float)m_height);
        setActiveCamera(&camera);
        trajectoryRenderer = LineRenderer(&camera);

        mainShader = Shader("default.vs", "default.fs");
        linesShader = Shader("debug_lines.vs", "debug_lines.fs");

        light = DirectionalLight({0, -1, -1, 0}, {.1, .1, .1, 1}, {1., 1., 1., 1.}, {1., 1., 1., 1.});

        // quick simulation tests
        // glm::vec3 pos(0, 4000, 0); // m
        // glm::vec3 prevSpeed(55, 0, 0);
        // glm::vec3 speed(55, 0, 0); //m/s
        // float mass = 80; // kg
        glm::vec3 gravity(0, -9.8f, 0) ; // m/s²
        // //gravity *= mass;
        // //glm::vec3 drag;
        //
        // glm::vec3 accel = gravity / mass;

        spring = new SpringJoint(&simulatedSpheres[0]->m_rb, &simulatedSpheres[1]->m_rb, 1.f, 1.f);

        solver = PhysicsSolver(simulatedSpheres, gravity);
        solver.addJoint(spring);

        return 0;
    }

    int render() override
    {
        glEnable(GL_DEPTH_TEST);
        glClearColor(.1f, .1f, .1f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("PhysicsTests");
            // ImGui::PlotLines("height", heightEvolution, IM_ARRAYSIZE(heightEvolution), 0, NULL, 0.f, 4000.f, ImVec2(300, 200));
            // ImGui::PlotLines("speed", speedEvolution, IM_ARRAYSIZE(speedEvolution), 0, NULL, -20.f, 0.f, ImVec2(300, 200));
            if(ImGui::Button("Toggle Simulation"))
            {
                if (isSimulationRunning)
                {
                    solver.SetRunningState(false);
                    physicsThread.join();
                }
                else
                {
                    solver.SetRunningState(true);
                    physicsThread = std::thread(&PhysicsSolver::run, std::ref(solver));
                }
                isSimulationRunning = !isSimulationRunning;
            }
        ImGui::End();

        ImGui::Begin("Inspector");
        for(auto& e : simulatedSpheres)
        {
            e->drawInspector();
        }
        ImGui::End();

        for(const auto &e : simulatedSpheres)
        {
            glm::mat4 model = e->getTransform().getModelMatrix();
            glm::mat4 view = mainCamera->view();
            glm::mat4 projection = mainCamera->projection();
            glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));
            glm::mat4 inverseView = glm::inverse(view);

            mainShader.use();
            mainShader.uniform_data("modelMatrix", model);
            mainShader.uniform_data("viewMatrix", view);
            mainShader.uniform_data("projectionMatrix", projection);
            mainShader.uniform_data("normalMatrix", normalMatrix);
            mainShader.uniform_data("inverseViewMatrix", inverseView);

            light.send_to_shader(mainShader, 0);

            e->draw(mainShader);
        }

        // draw a line between the two rbs
        trajectoryRenderer.addLine(rb.GetPosition(), rb2.GetPosition());

        linesShader.use();
        linesShader.uniform_data("color", glm::vec4(1, 0, 0, 1));
        trajectoryRenderer.draw(linesShader);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        return 0;
    }

    int quit() override
    {
        delete spring;
        physicsThread.join();
        return 0;
    }

protected:
    std::vector<PhysicsEntity*> simulatedSpheres;
    Camera camera;
    LineRenderer trajectoryRenderer;
    Shader mainShader;
    Shader linesShader;
    DirectionalLight light;

    Rigidbody rb;
    Rigidbody rb2;
    SpringJoint* spring;

    std::thread physicsThread;
    PhysicsSolver solver;

    bool isSimulationRunning{};

    float speedEvolution[1000];
    float heightEvolution[1000];
};

int main()
{
    PhysicsApp app;
    app.run();
    return 0;
}