#version 410

in vec3 frag_normal;
in vec3 frag_tangent;
in vec3 frag_bitangent;
in vec2 frag_texcoord;
in vec4 frag_position;

out vec4 frag_colour;

uniform sampler2D perlin_texture;
uniform sampler2D water_texture;
uniform sampler2D grass_texture;
uniform sampler2D snow_texture;
uniform float fPerlinScale;
uniform float fHighest;
uniform float fLowest;
uniform vec3 eye_pos;
uniform vec3 light_dir;

void main()
{
	float fCurrentPerlin = texture(perlin_texture, frag_texcoord).r;
	float fIntensity = ((fCurrentPerlin - fLowest) / (fHighest - fLowest));// get the normalised Perlin value

	int iTextureNum = int(fIntensity * 3.0f);	//	should give 0, 1, or 2 as the result
	//iTextureNum = 1;
	vec2 newTexCoord = frag_texcoord * 8.0f;
	vec3 material_colour;
	switch (iTextureNum)
	{
	case 0:
		{
			material_colour = texture(water_texture, newTexCoord).xyz;
			break;
		}
	case 1:
		{
			material_colour = texture(grass_texture, newTexCoord).xyz;
			break;
		}
	case 2:
		{
			material_colour = texture(snow_texture, newTexCoord).xyz;
			break;
		}
	default:
		{
			material_colour = texture(perlin_texture, newTexCoord).rrr;
			break;
		}
	}

	//	now calculate the lighting

	//	add uniforms for these later ...
	vec3 ambient_light = vec3(0.1f, 0.1f, 0.1f);
	vec3 light_colour = vec3(1.0f, 1.0f, 1.0f);
	float specular_power = 20.0f;

	vec3	ambient = material_colour * ambient_light;
	//material_colour = vec3(1.0f, 1.0f, 1.0f);
	vec3	N = normalize(frag_normal);
	vec3	L = normalize(light_dir);

	float	d = max(0.0f, dot(N, -L));
	vec3 diffuse = (vec3(d) * light_colour * material_colour);
	
	vec3	E = normalize(eye_pos - frag_position.xyz);
	//vec3	E = normalize(eye_pos - frag_position.xyz);
	vec3	R = reflect(-L, N);

	float	s = max(0.0f, dot(R, E));
	//s = 0.5f;
	s = pow(s, specular_power);
	vec3	specular = vec3(s) * light_colour * material_colour;

	frag_colour = vec4(ambient + diffuse + specular, 1);
	//frag_colour = vec4(ambient + diffuse, 1);
	//frag_colour = vec4(ambient, 1);
	//frag_colour = vec4(diffuse, 1);
	//frag_colour = vec4(specular, 1);
	//frag_colour = vec4(frag_normal, 1);
	//frag_colour = vec4(frag_tangent, 1);
	//frag_colour = vec4(frag_bitangent, 1);
	//frag_colour = vec4(N, 1);
	//frag_colour = vec4(0.0f, 0.0f, s, 1);
	//frag_colour = vec4(0.0f, d, 0.0f, 1);
	//frag_colour = vec4(0.0f, d, s, 1);
	//frag_colour = vec4(light_colour, 1);
	//frag_colour = vec4(material_colour, 1);
}
