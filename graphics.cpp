#include <GL/glew.h>
#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <iostream>

#include "graphics.hpp"

auto initGraphics() -> void {
  GLenum error = glewInit();
	if(error != GLEW_OK) {
		std::cerr << "Error starting GLEW: " << glewGetErrorString(error) << std::endl;
		exit(0);
	}

  glLineWidth(2.5);
}

auto createIndexBuffer(std::vector<GLuint> arr) -> GLuint {
  GLuint buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, arr.size()*sizeof(GLuint), arr.data(), GL_STATIC_DRAW);

  return buffer;
}

auto createVertexBuffer(std::initializer_list<std::vector<GLfloat>> arrays) -> GLuint {
  GLuint buffer;

  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);

  // reduce size
  std::vector<GLfloat>::size_type size = 0;
  for (const auto& arr : arrays) {
    size += arr.size();
  }

  glBufferData(GL_ARRAY_BUFFER, size*sizeof(GLfloat), NULL, GL_STATIC_DRAW);

  // create subbuffers
  std::vector<GLfloat>::size_type cur_offset = 0;
  for (const auto& arr : arrays) {
    glBufferSubData(GL_ARRAY_BUFFER, cur_offset*sizeof(GLfloat), arr.size()*sizeof(GLfloat), arr.data());
    cur_offset += arr.size();
  }

  return buffer;
}

auto createVertexAttrib(std::string_view name, GLuint program, GLuint size, GLuint offset) -> GLint {
  GLint attrib = glGetAttribLocation(program, name.data());
  glVertexAttribPointer(attrib, size, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(offset));
  glEnableVertexAttribArray(attrib);

  return attrib;
}
