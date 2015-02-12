#version 410

layout (location=0) in vec4 position;
layout (location=1) in vec4 normal;

out vec4 frag_normal;

uniform mat4 projection_view;

void main()
{
	frag_normal = normal;
	gl_Position = projection_view * position;
}
