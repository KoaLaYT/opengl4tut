#version 410 core

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 texture_st;

out vec2 texture_coordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(vertex_position, 0.0, 1.0);
  texture_coordinate = texture_st;
}
