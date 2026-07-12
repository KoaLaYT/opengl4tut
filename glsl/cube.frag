#version 410

in vec3 frag_view_normal;
in vec3 frag_view_position;
in vec3 light_view_position;

out vec4 frag_color;

struct Material {
  vec3  ambient;
  vec3  diffuse;
  vec3  specular;
  float shininess;
};

uniform Material material;

struct Light {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
uniform Light light;

void main() {
  vec3 ambient = material.ambient * light.ambient;

  vec3 norm = normalize(frag_view_normal);

  vec3 light_dir = normalize(light_view_position - frag_view_position);
  float diffuse_strength = max(dot(norm, light_dir), 0.0);
  vec3 diffuse = material.diffuse * diffuse_strength * light.diffuse;

  vec3 view_dir = normalize(-frag_view_position);
  vec3 reflect_dir = reflect(-light_dir, norm);
  float specular_strength = pow(max(dot(reflect_dir, view_dir), 0.0), material.shininess);
  vec3 specular = material.specular * specular_strength * light.specular;

  vec3 result = ambient + diffuse + specular;
  frag_color = vec4(result, 1.0);
}
