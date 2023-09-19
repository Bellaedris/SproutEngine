#include <glad/glad.h>
#include <GLFW/glfw3.h>

typedef GLFWwindow* Window;

Window create_window(int width, int height, int major, int minor);

void framebuffer_size_callback(GLFWwindow* w, int width, int height);
void processInput(GLFWwindow* w);
