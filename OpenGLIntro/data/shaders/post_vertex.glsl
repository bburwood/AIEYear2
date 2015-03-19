//	post processing vertex shader
#version 410

layout (location=0) in vec4 position;
layout (location=1) in vec2 texcoord;

out vec2 frag_texcoord;

void main()
{
	gl_Position = position;
	frag_texcoord = texcoord;
}
