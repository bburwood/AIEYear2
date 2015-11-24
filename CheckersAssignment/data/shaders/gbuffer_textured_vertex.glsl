#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 normal;
layout (location = 2) in vec2 texCoord;

out vec4 viewspace_position;
out vec4 viewspace_normal;
out vec2 frag_texcoord;

uniform mat4 view;
uniform mat4 view_proj;
uniform mat4 worldTransform;

void main()
{
	viewspace_position = view * worldTransform * vec4(position.xyz, 1);
	viewspace_normal = view * vec4(normal.xyz, 0);
	frag_texcoord = texCoord;
	gl_Position = view_proj * worldTransform * position;
}
