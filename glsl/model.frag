#version 410

in vec3 frag_view_normal;
in vec3 frag_view_position;
in vec2 tex_coords;

out vec4 frag_color;

uniform mat4 view;

struct Material {
  sampler2D texture_diffuse1;
  sampler2D texture_specular1;
  float     shininess;
};
uniform Material material;

struct DirLight {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  vec3 direction;
};
uniform DirLight dir_light;

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 view_dir, vec3 diffuse_color, vec3 specular_color);

void main() {
  vec3 normal = normalize(frag_view_normal);
  vec3 view_dir = normalize(-frag_view_position);
  vec3 diffuse_color = vec3(texture(material.texture_diffuse1, tex_coords));
  vec3 specular_color = vec3(texture(material.texture_specular1, tex_coords));

  vec3 result = calc_dir_light(dir_light, normal, view_dir, diffuse_color, specular_color);

  frag_color = vec4(result, 1.0);
}

float calc_diffuse_strength(vec3 normal, vec3 light_view_dir) {
  return max(dot(normal, light_view_dir), 0.0);
}

float calc_specular_strength(vec3 normal, vec3 light_view_dir, vec3 view_dir) {
  vec3 reflect_dir = reflect(-light_view_dir, normal);
  return pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
}

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 view_dir, vec3 diffuse_color, vec3 specular_color) {
  vec3 light_view_dir = normalize(-vec3(view * vec4(light.direction, 0.0)));

  float diffuse_strength = calc_diffuse_strength(normal, light_view_dir);
  float specular_strength = calc_specular_strength(normal, light_view_dir, view_dir);

  vec3 ambient = light.ambient * diffuse_color;
  vec3 diffuse = light.diffuse * diffuse_color * diffuse_strength;
  vec3 specular = light.specular * specular_color * specular_strength;

  return ambient + diffuse + specular;
}
