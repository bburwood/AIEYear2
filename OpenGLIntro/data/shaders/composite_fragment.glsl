#version 410

in vec2 frag_texcoord;

out vec4 out_colour;

uniform sampler2D albedo_tex;
uniform sampler2D light_tex;
uniform sampler2D normals_tex;
//uniform sampler2D position_tex;

uniform vec4 ambientLight;
uniform vec4 backgroundColor;

//	now the colour blindness modifiers
uniform float fRedWeakVal;
uniform float fGreenWeakVal;

void main()
{
	vec4 albedo_sample = texture(albedo_tex, frag_texcoord);
	vec4 light_sample = texture(light_tex, frag_texcoord);
	vec4 ambientComponent = albedo_sample * ambientLight;
//	vec4 tempColour = texture(normals_tex, frag_texcoord);
//	vec2	temp = vec2(tempColour.y, tempColour.z);
//	vec2	temp = normalize(tempColour.xy + tempColour.zw);

	vec4 tempColour = (albedo_sample * light_sample) + ambientComponent;
	//	add in Colour Blindness modifications
	tempColour.r = fRedWeakVal * tempColour.r + (1.0f - fRedWeakVal) * tempColour.g;
	tempColour.g = fGreenWeakVal * tempColour.g + (1.0f - fGreenWeakVal) * tempColour.r;


//	out_colour = texture(normals_tex, temp);
//	out_colour = texture(normals_tex, frag_texcoord);
//	out_colour = vec4(0, frag_texcoord, 1);

	out_colour = mix(backgroundColor, tempColour, albedo_sample.a);

//	out_colour = (albedo_sample * light_sample) + ambientComponent;
//	out_colour = albedo_sample * light_sample;
//	out_colour = ambientComponent;
//	out_colour = albedo_sample;
//	out_colour = light_sample;
//	out_colour = texture(normals_tex, frag_texcoord);
//	out_colour = vec4(1, 1, 1, 1);
}
