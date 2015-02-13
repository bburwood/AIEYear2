#version 410

in vec3 frag_normal;
in vec3 frag_position;
in vec3 frag_tangent;
in vec3 frag_bitangent;
in vec2 frag_texcoord;

out vec4 frag_colour;

uniform vec3 light_dir;
uniform vec3 light_colour;
uniform vec3 ambient_light;
uniform vec3 material_colour;

uniform vec3 eye_pos;
uniform float specular_power;
uniform float timer;

uniform sampler2D diffuse_tex;
uniform sampler2D normal_tex;
uniform sampler2D specular_tex;

void main()
{
	mat3 TBN = mat3(normalize(frag_tangent), normalize(frag_bitangent), normalize(frag_normal));
	vec3 sampled_normal = texture(normal_tex, frag_texcoord).xyz;
	vec3 adjusted_normal = sampled_normal * 2 - 1;

	vec3	colourModifier = vec3(abs(sin(timer)), abs(sin(timer)), abs(cos(timer)));
	vec3	ambient = material_colour * ambient_light * colourModifier;

	vec3	N = normalize(TBN * adjusted_normal);
	vec3	L = normalize(light_dir);
	vec3	material_colour = texture(diffuse_tex, frag_texcoord).xyz;

	float	d = max(0.0, dot(N, -L));
	vec3 diffuse = (vec3(d) * light_colour * material_colour) * colourModifier;

	vec3	E = normalize(eye_pos - frag_position);
	vec3	R = reflect(L, N);

	float	s = max(0.0, dot(R, E));
	vec3 material_specular = texture(specular_tex, frag_texcoord).xyz;
	s = pow(s, specular_power);
	vec3	specular = vec3(s) * light_colour * material_colour * colourModifier;

	frag_colour = vec4(ambient + diffuse + specular, 1);

	//	debugging light components with the following:
	//frag_colour = vec4(ambient, 1);
	//frag_colour = vec4(diffuse, 1);
	//frag_colour = vec4(specular, 1);
	//frag_colour = frag_normal;
}
