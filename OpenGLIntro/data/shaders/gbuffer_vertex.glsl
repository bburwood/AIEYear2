#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;

out vec4 viewspace_position;
out vec4 viewspace_normal;

uniform mat4 view;
uniform mat4 view_proj;

void main()
{
	mat4 mScale = mat4(	vec4(4.0f, 0.0f, 0.0f, 0.0f),
						vec4(0.0f, 4.0f, 0.0f, 0.0f),
						vec4(0.0f, 0.0f, 4.0f, 0.0f),
						vec4(0.0f, 0.0f, 0.0f, 1.0f));
	vec4 newPos = (mScale * position) + vec4(0, 1.5f, 0, 0);
	viewspace_position = view * vec4(newPos.xyz, 1);
//	viewspace_position = view * vec4(position.xyz, 1);
	viewspace_normal = view * vec4(normal.xyz, 0);

//	gl_Position = view_proj * position;
//	gl_Position = view_proj * mScale * newPos;
	gl_Position = view_proj * newPos;
}
