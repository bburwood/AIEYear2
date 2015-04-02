#version 410

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec3 normal;

out vec4 world_pos;
out vec4 world_normal;

uniform mat4 proj_view;
//uniform mat4 model;
//uniform mat4 invTransposeModel;

void main()
{
//	world_pos = model * position;
//	world_normal = invTransposeModel * normal;
	world_pos = vec4(position, 1);
	world_normal = vec4(normal, 0);
	gl_Position = proj_view * world_pos;
}
