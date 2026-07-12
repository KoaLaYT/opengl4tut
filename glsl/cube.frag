#version 410

in vec3 frag_view_normal;
in vec3 frag_view_position;
in vec2 tex_coords;

out vec4 frag_color;

uniform mat4 view;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
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

struct PointLight {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};
#define NR_POINT_LIGHTS 4
uniform PointLight point_lights[NR_POINT_LIGHTS];

struct FlashLight {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float inner_cutoff;
  float outer_cutoff;
};
uniform FlashLight flash_light;

vec3 calc_dir_light  (DirLight   light, vec3 normal, vec3 view_dir, vec3 diffuse_color, vec3 specular_color);
vec3 calc_point_light(PointLight light, vec3 normal, vec3 view_dir, vec3 diffuse_color, vec3 specular_color);
vec3 calc_flash_light(FlashLight light, vec3 normal, vec3 view_dir, vec3 diffuse_color, vec3 specular_color);

void main() {
  vec3 normal = normalize(frag_view_normal);
  vec3 view_dir = normalize(-frag_view_position);
  vec3 diffuse_color = vec3(texture(material.diffuse, tex_coords));
  vec3 specular_color = vec3(texture(material.specular, tex_coords));

  vec3 result = calc_dir_light(dir_light, normal, view_dir, diffuse_color, specular_color);
  for (int i = 0; i < NR_POINT_LIGHTS; i++) {
    result += calc_point_light(point_lights[i], normal, view_dir, diffuse_color, specular_color);
  }
  result += calc_flash_light(flash_light, normal, view_dir, diffuse_color, specular_color);
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

vec3 calc_point_light(PointLight light, vec3 normal, vec3 view_dir, vec3 diffuse_color, vec3 specular_color) {
  vec3 light_view_pos = vec3(view * vec4(light.position, 1.0));
  vec3 light_view_dir = normalize(light_view_pos - frag_view_position);

  float diffuse_strength = calc_diffuse_strength(normal, light_view_dir);
  float specular_strength = calc_specular_strength(normal, light_view_dir, view_dir);

  vec3 ambient = light.ambient * diffuse_color;
  vec3 diffuse = light.diffuse * diffuse_color * diffuse_strength;
  vec3 specular = light.specular * specular_color * specular_strength;

  float distance = length(light_view_pos - frag_view_position);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

  return (ambient + diffuse + specular) * attenuation;
}

vec3 calc_flash_light(FlashLight light, vec3 normal, vec3 view_dir, vec3 diffuse_color, vec3 specular_color) {
  vec3 light_view_dir = normalize(-frag_view_position);

  float diffuse_strength = calc_diffuse_strength(normal, light_view_dir);
  float specular_strength = calc_specular_strength(normal, light_view_dir, view_dir);

  vec3 ambient = light.ambient * diffuse_color;
  vec3 diffuse = light.diffuse * diffuse_color * diffuse_strength;
  vec3 specular = light.specular * specular_color * specular_strength;

  float theta = dot(-light_view_dir, vec3(0,0,-1));
  float epsilon = light.inner_cutoff - light.outer_cutoff;
  float intensity = clamp((theta - light.outer_cutoff)/epsilon, 0.0, 1.0);

  return ambient + (diffuse + specular) * intensity;
}
