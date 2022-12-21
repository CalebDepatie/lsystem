#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string_view>
#include <initializer_list>

auto initGraphics() -> void;
auto createIndexBuffer(std::vector<GLuint> arr) -> GLuint;
auto createVertexBuffer(std::initializer_list<std::vector<GLfloat>> arrays) -> GLuint;
auto createVertexAttrib(std::string_view name, GLuint program, GLuint size, GLuint offset) -> GLint;
auto drawLine(glm::vec3 from, glm::vec3 to) -> void;
