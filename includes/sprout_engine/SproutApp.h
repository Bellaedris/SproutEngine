#pragma once
#include "window.h"
#include "shader.h"
#include "camera.h"

#include <chrono>

class SproutApp
{
protected:
	Window window;
	Camera cam;

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
	SproutApp(int width, int height, int major = 3, int minor = 3)
		: delta_time(0.f), last_frame(0.f)
	{
		window = create_window(width, height, major, minor);

		lastX = width / 2;
		lastY = height / 2;

		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
	};

	~SproutApp()
	{
		glfwTerminate();
	};

	int run()
	{
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
			frame_begin = std::chrono::high_resolution_clock::now();
			glBeginQuery(GL_TIME_ELAPSED, frame_time_gpu);

			// handle camera inputs
			prerender();

			processInput(window);

			if (render() < 0)
				break;

			frame_end = std::chrono::high_resolution_clock::now();
			cpu_last_frame = std::chrono::duration_cast<std::chrono::milliseconds>(frame_end - frame_begin).count();
			glEndQuery(GL_TIME_ELAPSED);
			glGetQueryObjecti64v(frame_time_gpu, GL_QUERY_RESULT, &gpu_last_frame);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		if (quit() < 0)
			return -1;

		glDeleteQueries(1, &frame_time_gpu);

		return 0;
	};

	virtual int init() = 0;
	virtual int render() = 0;
	virtual int quit() = 0;
	void prerender()
	{
		//keyboard
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cam.process_input(FORWARD, delta_time);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cam.process_input(BACKWARD, delta_time);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cam.process_input(LEFT, delta_time);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cam.process_input(RIGHT, delta_time);
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			cam.process_input(UP, delta_time);
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			cam.process_input(DOWN, delta_time);

		//mouse
		if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			cam.process_mouse_movement(xoffset, yoffset);
		}
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	};

	static void mouse_callback(GLFWwindow* w, double xpos, double ypos)
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
	;
	static void scroll_callback(GLFWwindow* w, double xoffset, double yoffset)
	{
		fov -= (float)yoffset;
		if (fov < 1.0f)
			fov = 1.0f;
		if (fov > 45.0f)
			fov = 45.0f;
	};
};

float SproutApp::lastX = 0.f;
float SproutApp::lastY = 0.f;
float SproutApp::xoffset = 0.f;
float SproutApp::yoffset = 0.f;
float SproutApp::fov = 45.f;
bool SproutApp::firstMouse = true;