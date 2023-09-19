#pragma once
#include "window.h"
#include "shader.h"
#include "camera.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class SproutApp
{
protected:
	Window window;
	Camera cam;

	float delta_time;
	float last_frame;

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

	static void mouse_callback(GLFWwindow* w, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* w, double xoffset, double yoffset);
};

