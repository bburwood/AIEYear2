#version 410

in vec4 frag_position;
in vec4 frag_normal;
//in vec2 lfragTexCoord;

out vec4 frag_colour;

//uniform sampler2D albedoTexture;
uniform vec3 light_dir;
uniform vec3 light_colour;
uniform vec3 material_colour;
uniform vec3 ambient_light;

uniform vec3 eye_pos;
uniform float specular_power;
//uniform float timer;

void main()
{
	//vec2 lnewTexCoord = lfragTexCoord;

	vec3	ambient = material_colour * ambient_light;
	//vec3	tmaterial_colour = vec3(1.0f, 1.0f, 1.0f);
	vec3	N = normalize(frag_normal.xyz);
	vec3	L = normalize(light_dir);
	//vec3	L = normalize(vec3(0.0f, -10.0f, 0.0f));

	float	d = max(0.0, dot(N, -L));
	vec3 diffuse = (vec3(d) * light_colour * material_colour);

	vec3	E = normalize(eye_pos - frag_position.xyz);
	vec3	R = reflect(L, N);

	float	s = max(0.0, dot(R, E));
	s = pow(s, specular_power);

	vec3	specular = vec3(s) * light_colour * material_colour;

	frag_colour = vec4(ambient + diffuse + specular, 1);

	//	debugging light components with the following:
	//frag_colour = vec4(ambient, 1);
	//frag_colour = vec4(ambient_light, 1);
	//frag_colour = vec4(diffuse, 1);
	//frag_colour = vec4(light_colour, 1);
	//frag_colour = vec4(light_dir, 1);
	//frag_colour = vec4(N, 1);
	//frag_colour = vec4(E, 1);
	//frag_colour = vec4(R, 1);
	//frag_colour = vec4(d, d, d, 1);
	//frag_colour = vec4(specular, 1);
	//frag_colour = vec4(normalize(frag_normal.xyz), 1);
	//frag_colour = frag_position;
	//frag_colour = vec4(material_colour, 1);
}
