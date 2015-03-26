#version 410

out vec3 light_output;

uniform vec3 light_view_position;
uniform vec3 light_diffuse;
uniform float light_radius;

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
	light_output = d * falloff * light_diffuse;
//	light_output = vec3(1, 1, 1);	//	debug white point light 
//	light_output = light_diffuse;	//	shows the light box / sphere *AND* its colour when combined with just light_sample in the composite fragment shader
//	light_output = normal_sample;
//	light_output = position_sample;
//	light_output = to_light;
//	light_output = L;
}
