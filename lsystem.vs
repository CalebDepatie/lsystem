#version 330

in vec4 vPosition;
in vec3 vColour;
uniform mat4 projection;
uniform mat4 modelView;
out vec3 colour;

void main() {
  //gl_PointSize = 10.0;
  colour = vColour;
  gl_Position = projection * modelView * vPosition;
}
