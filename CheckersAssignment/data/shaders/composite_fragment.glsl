#version 410

in vec2 frag_texcoord;

out vec4 out_colour;

uniform sampler2D position_tex;
uniform sampler2D normals_tex;
uniform sampler2D albedo_tex;
uniform sampler2D light_tex;
uniform	float	fSpecPower;
uniform vec3 eye_pos;
uniform vec3 ambient_light;

void main()
{
	vec4 albedo_sample = texture(albedo_tex, frag_texcoord);
	vec4 light_sample = texture(light_tex, frag_texcoord);
//	vec4 normalSample = texture(normals_tex, frag_texcoord);
//	vec4 positionSample = texture(position_tex, frag_texcoord);
//	vec2	temp = vec2(tempColour.y, tempColour.z);
//	vec2	temp = normalize(tempColour.xy + tempColour.zw);
//	out_colour = texture(normals_tex, temp);
//	out_colour = texture(normals_tex, frag_texcoord);
//	out_colour = vec4(0, frag_texcoord, 1);
/*

	vec3	ambient = albedo_sample.xyz * ambient_light;
	vec3	N = normalize(normalSample.xyz);
	vec3	E = normalize(eye_pos - positionSample.xyz);
	//vec3	L = normalize(light_sample.xyz);	//	hmmm ... light direction ... how do we get this properly ...
	vec3	L = normalize(reflect(E, N));	//	hmmm ... light direction ... how do we get this properly ...

	vec3	R = reflect(L, N);

	float	d = max(0.0, dot(N, -L));
	vec3 diffuse = vec3(d) * (light_sample * albedo_sample).xyz;

	float	s = max(0.0, dot(R, E));
	//float s = d;
	s = pow(s, fSpecPower);

	vec3	specular = vec3(s) * light_sample.xyz * albedo_sample.xyz;
*/
//	out_colour = vec4(ambient + diffuse + specular, 1);
	
//	out_colour = vec4(ambient, 1);
//	out_colour = vec4(diffuse, 1);
//	out_colour = vec4(specular, 1);
//	out_colour = vec4(N, 1);
//	out_colour = vec4(L, 1);
//	out_colour = vec4(E, 1);
//	out_colour = vec4(R, 1);
//	out_colour = vec4(0, s, 0, 1);
//	out_colour = vec4(0, d, 0, 1);
//	out_colour = albedo_sample * light_sample;
//	out_colour = albedo_sample;
	out_colour = light_sample;
//	out_colour = normalSample;
//	out_colour = positionSample;
//	out_colour = vec4(ambient_light, 1.0f);
//	out_colour = texture(normals_tex, frag_texcoord);
//	out_colour = texture(position_tex, frag_texcoord);
//	out_colour = vec4(0, 0, fSpecPower, 1);
//	out_colour = vec4(1, 1, 1, 1);
}
