//	post processing fragment shader
#version 410

in vec2 frag_texcoord;

out vec4 frag_colour;

uniform sampler2D input_texture;
uniform float fTime;

vec4 BoxBlur()
{
	vec2 texelUV = 1.0f / textureSize(input_texture, 0).xy;

	vec4 colour = texture(input_texture, frag_texcoord);

	colour += texture(input_texture, frag_texcoord + texelUV);
	colour += texture(input_texture, frag_texcoord + vec2(texelUV.x, 0));
	colour += texture(input_texture, frag_texcoord + vec2(texelUV.x, -texelUV.y));
	colour += texture(input_texture, frag_texcoord + vec2(0, texelUV.y));
	colour += texture(input_texture, frag_texcoord + vec2(0, -texelUV.y));
	colour += texture(input_texture, frag_texcoord + vec2(-texelUV.x, texelUV.y));
	colour += texture(input_texture, frag_texcoord + vec2(-texelUV.x, 0));
	colour += texture(input_texture, frag_texcoord + vec2(-texelUV.x, -texelUV.y));
	colour /= 9.0f;
	return colour;
}

vec4 BasicGaussianBlur()
{
	//	only approximates a radius 1.5 Guassian Blur ...
	vec2 texelUV = 1.0f / textureSize(input_texture, 0).xy;
	float fQuarter = 0.25f;
	float fEighth = 0.125f;
	float fSixteenth = 0.0625f;

	vec4 colour = texture(input_texture, frag_texcoord) * fQuarter;

	colour += texture(input_texture, frag_texcoord + texelUV) * fEighth;
	colour += texture(input_texture, frag_texcoord + vec2(texelUV.x, 0)) * fEighth;
	colour += texture(input_texture, frag_texcoord + vec2(texelUV.x, -texelUV.y)) * fEighth;
	colour += texture(input_texture, frag_texcoord + vec2(0, texelUV.y)) * fEighth;
	colour += texture(input_texture, frag_texcoord + vec2(0, -texelUV.y)) * fSixteenth;
	colour += texture(input_texture, frag_texcoord + vec2(-texelUV.x, texelUV.y)) * fSixteenth;
	colour += texture(input_texture, frag_texcoord + vec2(-texelUV.x, 0)) * fSixteenth;
	colour += texture(input_texture, frag_texcoord + vec2(-texelUV.x, -texelUV.y)) * fSixteenth;

	return colour;
}

vec4 Distort(float fSize)
{
	vec2 mid = vec2(0.5f);
	float distanceFromCentre = distance(frag_texcoord, mid);
    vec2 normalizedCoord = normalize(frag_texcoord - mid);
    float bias = distanceFromCentre + sin(distanceFromCentre * fSize) * 0.02f;
    vec2 newCoord = mid + bias * normalizedCoord;
    return texture(input_texture, newCoord);
}

const float INVERSE_MAX_UINT = 1.0f / 4294967295.0f;

float rand(uint seed, float range)
{
	uint i = (seed ^ 12345391u) * 2654435769u;
	i ^= (i << 6u) ^ (i >> 26u);
	i *= 2654435769u;
	i += (i << 5u) ^ (i >> 12u);
	return float(range * i) * INVERSE_MAX_UINT;
}

vec4 RandomTexture()
{
	vec2 coord;
	uint seed = uint(fTime * 60.0f);
	coord.x = rand(uint((seed++) * frag_texcoord.x), 1.0f);
	coord.y = rand(uint((seed++) * frag_texcoord.y), 1.0f);
	return texture(input_texture, coord);
}

void main()
{
	vec4 finalColour;
	//finalColour = vec4(frag_texcoord, 0, 1);
	//finalColour = texture(input_texture, frag_texcoord);
//	finalColour = BoxBlur();
//	finalColour = BasicGaussianBlur();
//	finalColour = 0.5f * (BasicGaussianBlur() + Distort(25.0f * fTime));
//	finalColour = Distort(15.0f);
//	finalColour = Distort(fTime * 5.0f);
//	finalColour = vec4(1, 0, 0, 1);
	finalColour = ((Distort(sin(fTime * 10.0f) * 3) * 3) + BasicGaussianBlur()) * 0.25;
//	finalColour = vec4(0, frag_texcoord.x, frag_texcoord.y, 0);
	//	scan lines ...
	finalColour *= 0.75f + (0.25f * sin(1000.0f * frag_texcoord.y));
	//	following 2 lines do greyscale
	float greyLevel = 0.333334f * (finalColour.r + finalColour.g + finalColour.b);
	frag_colour = vec4(greyLevel, greyLevel, greyLevel, 1);

	//	normal final line
//	frag_colour = finalColour;
}
