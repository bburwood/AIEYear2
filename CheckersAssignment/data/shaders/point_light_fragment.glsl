#version 410

out vec3 light_output;

uniform vec3 light_view_position;
uniform vec3 light_diffuse;
uniform float light_radius;

uniform sampler2D position_tex;
uniform sampler2D normals_tex;
uniform sampler2D albedo_tex;
uniform	float	fSpecPower;
uniform vec3 eye_pos;
uniform vec3 ambient_light;

//uniform sampler2D position_texture;
//uniform sampler2D normal_texture;

void main()
{
	//	calculate the texture coordinates for the fragment
	vec2 texcoord = gl_FragCoord.xy / textureSize(position_tex, 0).xy;

	//	sample from textures
	vec4 positionSample = texture(position_tex, texcoord);
	vec4 normalSample = texture(normals_tex, texcoord);
	normalSample *= 2.0f;
	normalSample -= 1.0f;	//	make sure the normals are in the correct space: 0-1
	vec3	N = normalize(normalSample.xyz);

	//	compute light direction
	vec3 to_light = light_view_position - positionSample.xyz;
	vec3 L = normalize(to_light);

	//	compute lambertian term
	float d = max(0, dot(normalSample.xyz, L));
	//float d = max(0.0f, dot(N, L));

	//	compute simple linear falloff
	float falloff = 1.0f - min(1.0f, (length(to_light) / light_radius));

	//	output lambert * falloff * colour
	vec3	surfaceLighting = d * falloff * light_diffuse;

	////////	original shader above, computing the light falling on the surface ...
	////////	new deferred code below, calculating the Phong model for the current light ...
	vec4 albedo_sample = texture(albedo_tex, texcoord);

//	vec3	ambient = albedo_sample.xyz * ambient_light;
	vec3	E = normalize(vec3(0) - positionSample.xyz);
	//	L is already calculated above
	vec3	R = normalize(reflect(-L, N));

	//d = max(0.0, dot(N, L));
	//d = 0.5f;	//	for debugging!
	vec3 diffuse = vec3(d) * surfaceLighting * albedo_sample.xyz;

	float	s = max(0.0, dot(R, E));
	//float s = d;
	s = pow(s, fSpecPower);

	vec3	specular = vec3(s) * albedo_sample.xyz;	// * surfaceLighting;

//	light_output = ambient + diffuse + specular;
	//	ambient should only be added ONCE - not for every point light!
	light_output = diffuse + specular;


	//	debugs
//	light_output = vec3(1, 1, 1);	//	debug white point light 
//	light_output = light_diffuse;	//	shows the light box / sphere *AND* its colour when combined with just light_sample in the composite fragment shader
//	light_output = normalSample.xyz;
//	light_output = surfaceLighting;
//	light_output = albedo_sample.xyz;
//	light_output = ambient_light;
//	light_output = ambient;
//	light_output = diffuse;
//	light_output = specular;
//	light_output = N;
//	light_output = positionSample.xyz;
//	light_output = vec3(0, texcoord);
//	light_output = light_view_position;
//	light_output = to_light;
//	light_output = L;
//	light_output = R;
//	light_output = vec3(0, 0, light_radius);
//	light_output = vec3(0, 0, falloff);
//	light_output = vec3(d);
//	light_output = vec3(s);
//	light_output = vec3(1,0,0);
}
