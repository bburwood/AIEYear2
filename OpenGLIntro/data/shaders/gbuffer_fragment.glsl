#version 410

in vec4 viewspace_position;
in vec4 viewspace_normal;

layout (location = 0) out vec3 albedo;
layout (location = 1) out vec3 position;
layout (location = 2) out vec3 normal;

void main()
{
	position = viewspace_position.xyz;
	normal = normalize(viewspace_normal.xyz) * 0.5f + 0.5f;

	albedo = vec3(1);	//	just plain white for now - normally the raw unlit colour
	//albedo = viewspace_normal.xyz;
}
