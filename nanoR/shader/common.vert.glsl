#version 450
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 View;
uniform mat4 Proj;

void main() { 
  gl_Position = Proj * View * vec4(position, 1); 
}