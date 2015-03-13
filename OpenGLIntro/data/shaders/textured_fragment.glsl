#version 410
in vec2 frag_tex_coord;
in vec4 reflected_screen_pos;

out vec4 frag_colour;
uniform sampler2D diffuse;

void main()
{
	vec4 uv_position = reflected_screen_pos / reflected_screen_pos.w;
	uv_position = (uv_position + 1) * 0.5f;
//	frag_colour = texture(diffuse, frag_tex_coord);	//	original textured fragcolour line
	frag_colour = texture(diffuse, uv_position.xy);
};
