#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 color;
uniform float bias;
void main() {
  gl_Position = vec4(aPos.x + bias, aPos.y, aPos.z, 1.0f);
  color = aColor;
}