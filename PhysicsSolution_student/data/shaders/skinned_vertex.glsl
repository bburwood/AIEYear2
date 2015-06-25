#version 410
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;

out vec2 frag_tex_coord;
uniform mat4 projection_view;
uniform mat4 world;

uniform float time;
void main()
{
	frag_tex_coord = tex_coord;
	gl_Position = projection_view * world * position;
};
