#version 410

in vec2 frag_texcoord;

out vec4 out_colour;

uniform sampler2D albedo_tex;
uniform sampler2D light_tex;

//uniform sampler2D position_tex;
//uniform sampler2D normals_tex;

void main()
{
	vec4 albedo_sample = texture(albedo_tex, frag_texcoord);
	vec4 light_sample = texture(light_tex, frag_texcoord);
//	vec4 tempColour = texture(normals_tex, frag_texcoord);
//	vec2	temp = vec2(tempColour.y, tempColour.z);
//	vec2	temp = normalize(tempColour.xy + tempColour.zw);
//	out_colour = texture(normals_tex, temp);
//	out_colour = texture(normals_tex, frag_texcoord);
//	out_colour = vec4(0, frag_texcoord, 1);

	out_colour = albedo_sample * light_sample;
//	out_colour = albedo_sample;
//	out_colour = light_sample;
//	out_colour = vec4(1, 1, 1, 1);
}
