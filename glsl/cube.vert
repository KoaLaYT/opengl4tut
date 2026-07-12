#version 410

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 texture_coords;

out vec3 frag_view_normal;
out vec3 frag_view_position;
out vec2 tex_coords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(vertex_position, 1.0);
  frag_view_normal = mat3(transpose(inverse(view * model))) * vertex_normal;
  frag_view_position = vec3(view * model * vec4(vertex_position, 1.0));
  tex_coords = texture_coords;
}
