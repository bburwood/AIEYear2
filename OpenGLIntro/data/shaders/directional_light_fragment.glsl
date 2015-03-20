#version 410

in vec2 frag_texcoord;

out vec3 out_colour;

uniform vec3 light_dir;	//	this needs to be the view space light direction
uniform vec3 light_colour;

uniform sampler2D position_tex;
uniform sampler2D normals_tex;

void main()
{
	vec3 normal_sample = texture(normals_tex, frag_texcoord).xyz;
	normal_sample *= 2.0f;
	normal_sample -= 1.0f;

	vec3 position_sample = texture(position_tex, frag_texcoord).xyz;

//	vec3 N = normalize(normal_sample);
//	float d = max(0, dot(-light_dir, N));
	float d = max(0, dot(-light_dir, normal_sample));

	out_colour = light_colour * d;
}
