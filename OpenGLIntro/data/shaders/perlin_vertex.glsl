#version 410

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texcoord;

out vec2 frag_texcoord;

uniform mat4 view_proj;
uniform sampler2D perlin_texture;
uniform float fPerlinScale;
uniform float fHighest;
uniform float fLowest;

void main()
{
	vec4 pos = position;
//	pos.y += (texture(perlin_texture, texcoord).r * fPerlinScale);	//	standard working version
	pos.y += (texture(perlin_texture, texcoord).r * fPerlinScale / (fHighest - fLowest));

	frag_texcoord = texcoord;
	gl_Position = view_proj * pos;
}
