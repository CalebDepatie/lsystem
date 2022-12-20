#include <GL/glew.h>
#define GLFW_DLL
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <string>
#include <string_view>
#include <vector>
#include <tuple>
#include <stack>
#include <algorithm>
#include <iostream>
#include <bits/stdc++.h>

#include "shaders.h"
#include "callbacks.hpp"
#include "grammer.hpp"

GLuint lsystemVAO;
GLuint lsystemBuffer;
GLuint program;
int num_vertices = 0;

float cx, cy, cz;

constexpr bool BENCHMARK = false;

auto init(std::vector<actions> action_list, lsystem l) -> void;
auto display() -> void;
auto generateFromCommands(std::vector<actions> action_list, lsystem l);
auto getTime() -> double;

int main(int argc, char **argv) {

  if (argc < 2) {
    std::cerr << "Please provide the path to the LSystem Definition" << std::endl;
    exit(0);
  }

  auto lsystem = generateLSystem(argv[1]);

  auto tokens = lexString(lsystem);

  // setting error callback
  glfwSetErrorCallback(error_callback);

  // initialize glfw
	if (!glfwInit()) {
		fprintf(stderr, "can't initialize GLFW\n");
	}

  GLFWwindow* window = glfwCreateWindow(512, 512, "L-System", NULL, NULL);

  if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

  projection = glm::perspective(0.7f, 1.0f, 1.0f, 800.0f);

  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  glfwMakeContextCurrent(window);
	GLenum error = glewInit();
	if(error != GLEW_OK) {
		printf("Error starting GLEW: %s\n",glewGetErrorString(error));
		exit(0);
	}

  init(tokens, lsystem);

  glEnable(GL_DEPTH_TEST);
	glClearColor(1.0,1.0,1.0,1.0);
	glViewport(0, 0, 512, 512);

  glfwSwapInterval(1);

  while (!glfwWindowShouldClose(window)) {
		display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

  return 0;
}


auto generateFromCommands(std::vector<actions> action_list, lsystem l) {
  using head_state = std::tuple<glm::vec3, glm::vec3, GLuint>;
  // convert to radians
  GLfloat ANGLE_STEP = l.ANGLE_STEP * (3.14/180);
  GLfloat DIST_STEP = 1.0;

  auto vertices = std::vector<GLfloat>();
  auto indices = std::vector<GLuint>();

  auto cur_state = std::vector<head_state>();
  auto tree_stack = std::stack<std::vector<head_state>>();

  GLuint vertex_num = 0;

  // variable creation required outside constexpr as it doesn't flatten scope
  double start;
  if constexpr(BENCHMARK) {
    start = getTime();
  }

  // intiial setup
  vertices.emplace_back(0);
  vertices.emplace_back(0);
  vertices.emplace_back(0);

  vertex_num++;

  cur_state.push_back(std::make_tuple(
    glm::vec3(0,0,0),
    glm::vec3(0,1,0),
    0
  ));

  const auto addVertex = [&vertices](glm::vec3 head) {
    vertices.emplace_back(head.x);
    vertices.emplace_back(head.y);
    vertices.emplace_back(head.z);
  };

  for (const actions& cur_action : action_list) {
    switch(cur_action) {
      case push: {
          tree_stack.push(cur_state);

          break;
        }
      case pop: {
          auto old_state = tree_stack.top();
          tree_stack.pop();

          cur_state = old_state;

          break;
        }
      case move: {
        for (auto& [head, cur_dir, prev_vertex] : cur_state) {
          head += (cur_dir * DIST_STEP);
          addVertex(head);

          prev_vertex = vertex_num++;
        }

        break;// constexpr GLfloat ANGLE_STEP = 1.570796;

      }
      case forward: {
        for (auto& [head, cur_dir, prev_vertex] : cur_state) {
          head += (cur_dir * DIST_STEP);
          // std::cout << "x: " << head.x << std::endl;
          // std::cout << "y: " << head.y << std::endl << std::endl;
          addVertex(head);

          indices.emplace_back(prev_vertex);
          prev_vertex = vertex_num;
          indices.emplace_back(vertex_num++);
        }
        break;
      }

      case right_turn: {
        for (auto& [head, cur_dir, prev_vertex] : cur_state) {
          cur_dir = glm::rotate(cur_dir, ANGLE_STEP, glm::vec3(0,0,-1));
        }

        break;
      }

      case left_turn: {
        for (auto& [head, cur_dir, prev_vertex] : cur_state) {
          cur_dir = glm::rotate(cur_dir, -ANGLE_STEP, glm::vec3(0,0,-1));
        }

        break;
      }
      case turn_in: {
        for (auto& [head, cur_dir, prev_vertex] : cur_state) {
          cur_dir = glm::rotate(cur_dir, ANGLE_STEP, glm::vec3(1,0,0));
        }

        break;
      }
      case turn_out: {
        for (auto& [head, cur_dir, prev_vertex] : cur_state) {
          cur_dir = glm::rotate(cur_dir, -ANGLE_STEP, glm::vec3(1,0,0));
        }

        break;
      }
      case turn_y_in: {
        for (auto& [head, cur_dir, prev_vertex] : cur_state) {
          cur_dir = glm::rotate(cur_dir, -ANGLE_STEP, glm::vec3(0,1,0));
        }

        break;
      }
      case turn_y_out: {
        for (auto& [head, cur_dir, prev_vertex] : cur_state) {
          cur_dir = glm::rotate(cur_dir, -ANGLE_STEP, glm::vec3(0,1,0));
        }

        break;
      }
      case half_speed: {
        DIST_STEP /= 2.0f;
        break;
      }
      case double_speed: {
        DIST_STEP *= 2.0;
        break;
      }
      case split: {
        auto new_state = std::vector<head_state>();

        for (auto& [head, cur_dir, prev_vertex] : cur_state) {
          // create perpindicular heads
          // determine new directions
          const auto new_dir1 = glm::rotate(cur_dir, -ANGLE_STEP/2, glm::vec3(0,0,-1));
          const auto new_dir2 = glm::rotate(cur_dir, ANGLE_STEP/2, glm::vec3(0,0,-1));

          auto head1 = head + (new_dir1 * DIST_STEP);
          addVertex(head1);

          indices.emplace_back(prev_vertex);
          indices.emplace_back(vertex_num);

          new_state.push_back(std::make_tuple(
            head1,
            new_dir1,
            vertex_num++
          ));

          auto head2 = head + (new_dir2 * DIST_STEP);
          addVertex(head2);

          indices.emplace_back(prev_vertex);
          indices.emplace_back(vertex_num);

          new_state.push_back(std::make_tuple(
            head2,
            new_dir2,
            vertex_num++
          ));
        }

        cur_state = std::move(new_state);

        break;
      }
      default: {
        std::cerr << "Action undefined" << std::endl;
        exit(0);
      }
    };
  }

  num_vertices = indices.size();

  // determine centre
  float xmin, ymin, zmin, xmax, ymax, zmax;
  xmin = ymin = zmin = 1000000.0;
	xmax = ymax = zmax = -1000000.0;
	for(int i=0; i<vertices.size()/3; i++) {
		if(vertices[3*i] < xmin)
			xmin = vertices[3*i];
		if(vertices[3*i] > xmax)
			xmax = vertices[3*i];
		if(vertices[3*i+1] < ymin)
			ymin = vertices[3*i+1];
		if(vertices[3*i+1] > ymax)
			ymax = vertices[3*i+1];
		if(vertices[3*i+2] < zmin)
			zmin = vertices[3*i+2];
		if(vertices[3*i+2] > zmax)
			zmax = vertices[3*i+2];
	}
  const float rangex = xmax + xmin;
  const float rangey = ymax + ymin;
  const float rangez = zmax + zmin;

  cx = rangex/2.0f;
	cy = rangey/2.0f;
	cz = rangez/2.0f;

  // set zoom / eye position
  const float largest_edge = std::max<float>(fabs(xmax) + fabs(xmin), fabs(ymax) + fabs(ymin));
  zoom = (largest_edge / tan(0.7853982/2)) +1.0;

  if constexpr(BENCHMARK) {
    auto end = getTime();

    std::cout << "s " << start / CLOCKS_PER_SEC << std::endl;
    std::cout << "e " << end / CLOCKS_PER_SEC << std::endl;

    std::cout << "Creating vertices took: " << static_cast<double>(end - start) / CLOCKS_PER_SEC << std::endl;
  }

  return std::make_tuple(vertices, indices);
}

auto init(std::vector<actions> action_list, lsystem l) -> void {
  // glEnable(GL_PROGRAM_POINT_SIZE);
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glLineWidth(2.5);

  const auto [vertices, indices] = generateFromCommands(action_list, l);

  // for (const auto& index : indices) {
  //   std::cout << index << std::endl;
  // }

  // generate colour gradient
  // HSV conversions with the help of https://www.had2know.org/technology/hsv-rgb-conversion-formula-calculator.html
  auto colours = std::vector<GLfloat>();
  for (int i = 0; i < vertices.size()/2; i++) {
    const float H = (static_cast<float>(i)/(vertices.size()/2))*360;
    const float z = (1 - fabs(fmod((H/60), 2.0f) - 1));
    float R, G, B;

    if (H < 60) {
      R = 1;
      G = z;
      B = 0;
    } else if (H < 120) {
      R = z;
      G = 1;
      B = 0;
    } else if (H < 180) {
      R = 0;
      G = 1;
      B = z;
    } else if (H < 240) {
      R = 0;
      G = z;
      B = 1;
    } else if (H < 300) {
      R = z;
      G = 0;
      B = 1;
    } else if (H < 360) {
      R = 1;
      G = 0;
      B = z;
    }

    colours.push_back(R); // r
    colours.push_back(G); // g
    colours.push_back(B); // b
  }

  int vs = buildShader(GL_VERTEX_SHADER, (char*)"lsystem.vs");
  int fs = buildShader(GL_FRAGMENT_SHADER, (char*)"lsystem.fs");
  program = buildProgram(vs,fs,0);

  glGenVertexArrays(1, &lsystemVAO);
	glBindVertexArray(lsystemVAO);

  glGenBuffers(1, &lsystemBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lsystemBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

  GLuint vbuffer;
  glGenBuffers(1, &vbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
  glBufferData(GL_ARRAY_BUFFER, (vertices.size()+colours.size())*sizeof(GLfloat) , NULL, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(GLfloat), vertices.data());
  glBufferSubData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), colours.size()*sizeof(GLfloat), colours.data());

  glUseProgram(program);

  GLint vPosition = glGetAttribLocation(program,"vPosition");
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

  GLint vColour = glGetAttribLocation(program,"vColour");
	glVertexAttribPointer(vColour, 3, GL_FLOAT, GL_FALSE, 0, (void*) (vertices.size()*sizeof(GLfloat)));
	glEnableVertexAttribArray(vColour);
}

auto display() -> void {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program);

  eyex = sin(rotZ)*cos(rotX)*zoom + cx;
  eyey = sin(rotX)*zoom + cy;
  eyez = cos(rotZ)*cos(rotX)*zoom + cz;

  glm::mat4 view = glm::lookAt(glm::vec3(eyex, eyey, eyez),
	 glm::vec3(cx,cy,cz),
	 glm::vec3(0.0f, 1.0f, 0.0f));

  int modelViewLoc = glGetUniformLocation(program,"modelView");
	glUniformMatrix4fv(modelViewLoc, 1, 0, glm::value_ptr(view));

  int projectionLoc = glGetUniformLocation(program,"projection");
	glUniformMatrix4fv(projectionLoc, 1, 0, glm::value_ptr(projection));

  glBindVertexArray(lsystemVAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lsystemBuffer);

  glDrawElements(GL_LINES, num_vertices, GL_UNSIGNED_INT, NULL);
}

auto getTime() -> double {
  clock_t elapsed = clock();

  return static_cast<double>(elapsed);
}
