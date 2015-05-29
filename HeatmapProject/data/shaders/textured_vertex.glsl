#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec2 tex_coord;

uniform mat4 projection_view; 
out vec2 frag_tex_coord;

void main()
{
	frag_tex_coord = tex_coord;
	gl_Position = projection_view * position;
}