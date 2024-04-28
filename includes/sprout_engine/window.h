#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

typedef GLFWwindow* Window;

void framebuffer_size_callback(GLFWwindow* w, int width, int height);

Window create_window(int width, int height, int major, int minor);;

void processInput(GLFWwindow* w);
