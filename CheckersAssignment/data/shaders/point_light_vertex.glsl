#version 410

layout(location = 0) in vec4 position;

uniform vec3 light_position;	//	world position
uniform float light_radius;
uniform mat4 proj_view;

void main()
{
	gl_Position = proj_view * vec4((position.xyz * light_radius) + light_position, 1);
}
