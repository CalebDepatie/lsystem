#version 330

in vec3 colour;

void main() {
  gl_FragColor = vec4(colour, 1.0);
}
