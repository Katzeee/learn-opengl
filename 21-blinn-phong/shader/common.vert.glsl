#version 450 core

layout(location = 0) in vec3 lP;
layout(location = 1) in vec3 inN;
layout(location = 2) in vec2 inuv;

uniform mat4 Model;
layout(std140, binding = 0) uniform Matrices {
  mat4 View;
  mat4 Proj;
};

out VS_OUT {
  vec3 P;
  vec3 N;
  vec2 uv;
} vs_out;

void main() {
  gl_Position = Proj * View * Model * vec4(lP, 1);
  vs_out.P = (Model * vec4(lP, 1)).xyz;
  vs_out.uv = inuv;
  vs_out.N = inN;
}