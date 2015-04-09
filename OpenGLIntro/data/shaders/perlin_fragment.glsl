#version 410

in vec2 frag_texcoord;

out vec4 frag_colour;

uniform sampler2D perlin_texture;
uniform float fPerlinScale;
uniform float fHighest;
uniform float fLowest;

void main()
{
	float fCurrentPerlin = texture(perlin_texture, frag_texcoord).r;
	float fIntensity = ((fCurrentPerlin - fLowest) / (fHighest - fLowest));// /(fHighest + fLowest));
//	float fIntensity = ((fCurrentPerlin));// /(fHighest + fLowest));
	frag_colour = vec4(fIntensity, fIntensity, fIntensity, 1.0f);
//	frag_colour = vec4(0.05f, 0.6f, 0.7f, 1.0f);
}
