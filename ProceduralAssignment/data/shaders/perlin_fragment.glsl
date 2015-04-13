#version 410

in vec2 frag_texcoord;

out vec4 frag_colour;

uniform sampler2D perlin_texture;
uniform sampler2D water_texture;
uniform sampler2D grass_texture;
uniform sampler2D snow_texture;
uniform float fPerlinScale;
uniform float fHighest;
uniform float fLowest;

void main()
{
	float fCurrentPerlin = texture(perlin_texture, frag_texcoord).r;
	float fIntensity = ((fCurrentPerlin - fLowest) / (fHighest - fLowest));// get the normalised Perlin value

	int iTextureNum = int(fIntensity * 3.0f);	//	should give 0, 1, or 2 as the result
	//iTextureNum = 1;
	vec2 newTexCoord = frag_texcoord * 8.0f;
	switch (iTextureNum)
	{
	case 0:
		{
			frag_colour = vec4(texture(water_texture, newTexCoord).xyz, 1);
//			TextureToUse = water_texture;
			break;
		}
	case 1:
		{
			frag_colour = vec4(texture(grass_texture, newTexCoord).xyz, 1);
//			TextureToUse = grass_texture;
			break;
		}
	case 2:
		{
			frag_colour = vec4(texture(snow_texture, newTexCoord).xyz, 1);
//			TextureToUse = snow_texture;
			break;
		}
	default:
		{
			frag_colour = vec4(texture(perlin_texture, newTexCoord).rrr, 1);
//			TextureToUse = perlin_texture;
			break;
		}
	}

//	frag_colour = vec4(texture(water_texture, frag_texcoord).xyz, 1);
//	frag_colour = vec4(0.5f * fIntensity * iTextureNum, 0.75f * fIntensity * iTextureNum, 1.5f * fIntensity, 1.0f);
//	frag_colour = vec4(0.05f, 0.3f, 0.7f, 1.0f);
}
