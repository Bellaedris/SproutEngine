//
// Created by Bellaedris on 28/04/2024.
//

#include "SproutApp.h"

#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_glfw.h>
#include <IMGUI/imgui_impl_opengl3.h>

float SproutApp::lastX = 0.f;
float SproutApp::lastY = 0.f;
float SproutApp::xoffset = 0.f;
float SproutApp::yoffset = 0.f;
float SproutApp::fov = 45.f;
bool SproutApp::firstMouse = true;

SproutApp::SproutApp(int width, int height, int major, int minor)
        : delta_time(0.f), last_frame(0.f), m_width(width), m_height(height)
{
    window = create_window(width, height, major, minor);

    vendor = glGetString(GL_VENDOR);
    gpu = glGetString(GL_RENDERER);
    api = glGetString(GL_VERSION);

    lastX = width / 2;
    lastY = height / 2;

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    start_time = std::chrono::high_resolution_clock::now();
}

SproutApp::~SproutApp() {
    glfwTerminate();
}

void SproutApp::prerender() {
    //keyboard
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainCamera->process_input(FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainCamera->process_input(BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainCamera->process_input(LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainCamera->process_input(RIGHT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        mainCamera->process_input(UP, delta_time);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        mainCamera->process_input(DOWN, delta_time);

    //mouse
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mainCamera->process_mouse_movement(xoffset, yoffset);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true;
    }

    // init ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

int SproutApp::run() {
    if (init() < 0)
        return -1;

    glGenQueries(1, &frame_time_gpu);
    glEnable(GL_DEPTH_TEST);

    // glfw rendering loop
    while (!glfwWindowShouldClose(window))
    {
        float current_time = glfwGetTime();
        delta_time = current_time - last_frame;
        last_frame = current_time;

        glfwGetFramebufferSize(window, &m_width, &m_height);

        frame_begin = std::chrono::high_resolution_clock::now();
        timeSinceStartup = std::chrono::duration<float>(frame_begin - start_time).count();
        glBeginQuery(GL_TIME_ELAPSED, frame_time_gpu);

        // handle camera inputs
        prerender();

        processInput(window);

        if (render() < 0)
            break;

        ImGui::Begin("Performances", nullptr,  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
        ImGui::Text("SproutEngine version 0.0.1");
        ImGui::Text("OpenGL%s", api);
        ImGui::Text("%s %s", vendor, gpu);

        float average = 0.0f;
        for(float n : framerate)
            average += n;
        average /= (float)IM_ARRAYSIZE(framerate);
        ImGui::Text("Average %02fFPS", average);

        int milli = (int) (gpu_last_frame / 1000000);
        int micro = (int) ((gpu_last_frame / 1000) % 1000);
        ImGui::Text(
                "cpu %03dms\ngpu %02dms % 03dus",
                cpu_last_frame,
                milli, micro
        );
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        frame_end = std::chrono::high_resolution_clock::now();
        cpu_last_frame = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_begin).count();
        glEndQuery(GL_TIME_ELAPSED);
        glGetQueryObjecti64v(frame_time_gpu, GL_QUERY_RESULT, &gpu_last_frame); // should be changed, synchronizes GPU with CPU...
        framerate[values_offset] = 1.f / delta_time;
        values_offset = (values_offset + 1) % IM_ARRAYSIZE(framerate);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (quit() < 0)
        return -1;

    glDeleteQueries(1, &frame_time_gpu);

    return 0;
}

void SproutApp::mouse_callback(GLFWwindow *w, double xpos, double ypos) {
    if (glfwGetKey(w, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        xoffset = xpos - lastX;
        yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;
    }
}

void SproutApp::scroll_callback(GLFWwindow *w, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void SproutApp::setActiveCamera(Camera *p_camera) {
    mainCamera = p_camera;
}

glm::mat4 SproutApp::viewport() const {
    float w = (float)m_width / 2.f;
    float h = (float)m_height / 2.f;

    return {
            w,  0.,  0.,  0,
            0,  h,  0,   0,
            0., 0., .5f, 0,
            w,  h,  .5f,   1
            };
}
