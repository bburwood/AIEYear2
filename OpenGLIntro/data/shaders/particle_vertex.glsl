#version 410
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour;
out vec4 out_colour;
uniform mat4 projection_view;
//uniform float time;
void main()
{
	out_colour = colour;
	gl_Position = projection_view * position;
};
