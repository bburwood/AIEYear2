#version 410

out vec3 light_output;

uniform vec3 light_view_position;
uniform vec3 light_diffuse;
uniform float light_radius;
uniform float fSpecPower;

uniform sampler2D position_texture;
uniform sampler2D normal_texture;

void main()
{
	//	calculate the texture coordinates for the fragment
	vec2 texcoord = gl_FragCoord.xy / textureSize(position_texture, 0).xy;

	//	sample from textures
	vec3 position_sample = texture(position_texture, texcoord).xyz;
	vec3 normal_sample = texture(normal_texture, texcoord).xyz;
	normal_sample *= 2.0f;
	normal_sample -= 1.0f;	//	make sure the normals are in the correct space: 0-1

	//	compute light direction
	vec3 to_light = light_view_position - position_sample;
	vec3 L = normalize(to_light);

	//	compute lambertian term
	float d = max(0, dot(normal_sample, L));

	//	compute simple linear falloff
	float falloff = 1.0f - min(1.0f, (length(to_light) / light_radius));

	//	output lambert * falloff * colour
	vec3 surfaceLighting = d * falloff * light_diffuse;
	vec3 diffuse = vec3(d) * surfaceLighting;	//	* colour; // assume the colour is white

	vec3	E = normalize(vec3(0) - position_sample);
	//	L is already calculated above
	vec3	R = normalize(reflect(-L, normal_sample));

	float	s = max(0.0, dot(R, E));
	s = pow(s, fSpecPower);

	vec3	specular = vec3(s) * light_diffuse * vec3(0.35f); //* albedo_sample.xyz;	//	assume colour is dark for spec

	light_output = diffuse + specular;

//	light_output = diffuse;
//	light_output = specular;
//	light_output = vec3(1, 1, 1);	//	debug white point light 
//	light_output = light_diffuse;	//	shows the light box / sphere *AND* its colour when combined with just light_sample in the composite fragment shader
//	light_output = normal_sample;
//	light_output = position_sample;
//	light_output = surfaceLighting;
//	light_output = to_light;
//	light_output = L;
}
