#pragma once

inline glm::mat4 projection;
inline float eyex = 0.0;
inline float eyey = 0.0;
inline float eyez = 0.0;

inline float rotZ = 0.0;
inline float rotX = 0.0;
inline float zoom = 10.0;

auto error_callback(int error, const char* description) -> void;
auto framebufferSizeCallback(GLFWwindow *window, int w, int h) -> void;
auto key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) -> void;
