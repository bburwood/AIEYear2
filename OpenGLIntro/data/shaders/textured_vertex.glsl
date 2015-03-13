#version 410
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;

out vec2 frag_tex_coord;
out vec4 reflected_screen_pos;

uniform mat4 projection_view;
uniform mat4 projection_view_reflected;

uniform float time;
void main()
{
	reflected_screen_pos = projection_view_reflected * position;
	frag_tex_coord = tex_coord;
	gl_Position = projection_view * position;
};
