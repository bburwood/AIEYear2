#version 410

in vec4 frag_normal;
in vec4 shadow_coord;

out vec4 frag_colour;

uniform vec3 light_dir;
uniform sampler2D shadow_map;

void main()
{
	float d = max(0, dot(-light_dir, normalize(frag_normal).xyz));
	float distanceFromLight = texture(shadow_map, shadow_coord.xy).r;
	if (distanceFromLight < shadow_coord.z - 0.01f)
	{
		d = 0;
	}
	frag_colour = vec4(d, d, d, 1);
}
