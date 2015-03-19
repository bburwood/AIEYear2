#version 410

layout (location = 0) in vec3 position;

uniform mat4 light_matrix;

void main()
{
	gl_Position = light_matrix * vec4(position, 1);
}
