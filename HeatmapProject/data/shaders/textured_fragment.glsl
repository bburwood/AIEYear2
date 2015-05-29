#version 410

uniform sampler2D diffuse;

in vec2 frag_tex_coord;

out vec4 frag_color;

void main()
{
	frag_color = texture(diffuse, frag_tex_coord);
}