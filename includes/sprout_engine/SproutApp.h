#ifndef SPROUTENGINE_SPROUTAPP_H
#define SPROUTENGINE_SPROUTAPP_H

#include "window.h"
#include "shader.h"
#include "camera.h"

#include <IMGUI/imgui.h>
#include <IMGUI/imgui_impl_glfw.h>
#include <IMGUI/imgui_impl_opengl3.h>

#include <chrono>

class SproutApp
{
protected:
	Window window;
	Camera* cam;

	float delta_time;
	float last_frame;

	std::chrono::high_resolution_clock::time_point frame_begin;
	std::chrono::high_resolution_clock::time_point frame_end;
	GLuint frame_time_gpu;
	int cpu_last_frame = 0;
	GLint64 gpu_last_frame = 0;

	// mouse movement control
	static float lastX;
	static float lastY;
	static float xoffset;
	static float yoffset;
	static bool firstMouse;

	//scroll control
	static float fov;
public:
	SproutApp(int width, int height, int major = 3, int minor = 3);

	~SproutApp();

	int run();

	virtual int init() = 0;
	virtual int render() = 0;
	virtual int quit() = 0;
	void prerender();

    void setActiveCamera(Camera* p_camera);

	static void mouse_callback(GLFWwindow* w, double xpos, double ypos);

	static void scroll_callback(GLFWwindow* w, double xoffset, double yoffset);
};

#endif //SPROUTENGINE_SPROUTAPP_H
