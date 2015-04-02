#version 410

in vec4 frag_normal;
in vec4 frag_position;
out vec4 frag_colour;

uniform vec3 light_dir;
uniform vec3 light_colour;
uniform vec3 material_colour;
uniform vec3 ambient_light;

uniform vec3 eye_pos;
uniform float specular_power;
uniform float timer;

void main()
{
	vec3	colourModifier = vec3(abs(sin(timer)), abs(sin(timer)), abs(cos(timer)));
	vec3	ambient = material_colour * ambient_light * colourModifier;

	vec3	N = normalize(frag_normal.xyz);
	vec3	L = normalize(light_dir);

	float	d = max(0.0, dot(N, -L));
	vec3 diffuse = (vec3(d) * light_colour * material_colour) * colourModifier;

	vec3	E = normalize(eye_pos - frag_position.xyz);
	vec3	R = reflect(L, N);

	float	s = max(0.0, dot(R, E));
	s = pow(s, specular_power);
	vec3	specular = vec3(s) * light_colour * material_colour * colourModifier;

	frag_colour = vec4(ambient + diffuse + specular, 1);

	//	debugging light components with the following:
	//frag_colour = vec4(ambient, 1);
	//frag_colour = vec4(diffuse, 1);
	//frag_colour = vec4(specular, 1);
	//frag_colour = frag_normal;
}
