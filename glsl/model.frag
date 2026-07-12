#version 410

in vec2 texture_coordinate;

out vec4 frag_color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main() {
  frag_color = texture(texture_diffuse1, texture_coordinate);
}
