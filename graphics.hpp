#pragma once

#include <glm/glm.hpp>
#include "core/dyn_array.ipp"
#include <string_view>
#include <initializer_list>

auto initGraphics() -> void;
auto createIndexBuffer(velox::dyn_array<GLuint> arr) -> GLuint;
auto createVertexBuffer(std::initializer_list<velox::dyn_array<GLfloat>> arrays) -> GLuint;
auto createVertexAttrib(std::string_view name, GLuint program, GLuint size, GLuint offset) -> GLint;
auto drawLine(glm::vec3 from, glm::vec3 to) -> void;
