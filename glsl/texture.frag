#version 410

in vec2 texture_coordinate;

out vec4 frag_color;

uniform sampler2D basic_texture;

void main() {
  vec4 texel = texture(basic_texture, texture_coordinate);
  frag_color = texel;
}
