#include <GL/glew.h>
#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "callbacks.hpp"

auto error_callback(int error, const char* description) -> void {
	std::cerr << "OpenGL Error: " << description << std::endl;
}

auto framebufferSizeCallback(GLFWwindow *window, int w, int h) -> void {
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = 1.0f * w / h;

	glfwMakeContextCurrent(window);
	glViewport(0, 0, w, h);
	projection = glm::perspective(0.7f, ratio, 1.0f, 800.0f);

	// glm::lookAt()
}

auto key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
	constexpr float ZOOM_STEP = 0.5;
	constexpr float ROT_STEP = 0.2;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT))
		zoom -= ZOOM_STEP;
	if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT))
		zoom += ZOOM_STEP;


	if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
		rotZ -= ROT_STEP;
	if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
		rotZ += ROT_STEP;

	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
		rotX += ROT_STEP;
	if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
		rotX -= ROT_STEP;
}
