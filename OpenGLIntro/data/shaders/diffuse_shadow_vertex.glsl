#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec4 frag_normal;
out vec4 shadow_coord;

uniform mat4 view_projection;
uniform mat4 light_matrix;

void main()
{
	frag_normal = vec4(normal, 0);
	shadow_coord = light_matrix * vec4(position, 1);
	gl_Position = view_projection * vec4(position, 1);
}
