#version 410

in vec4 viewspace_position;
in vec4 viewspace_normal;

layout (location = 0) out vec3 albedo;
layout (location = 1) out vec3 position;
layout (location = 2) out vec3 normal;

uniform vec3 material_colour;

void main()
{
	position = viewspace_position.xyz;
	normal = normalize(viewspace_normal.xyz) * 0.5f + 0.5f;

	//albedo = vec3(1);	//	just plain white for now - normally the raw unlit colour
	//albedo = vec3(1.0f, 0.1f, 0.1f);	//	red test ...
	albedo = material_colour;
	//albedo = viewspace_normal.xyz;
}
