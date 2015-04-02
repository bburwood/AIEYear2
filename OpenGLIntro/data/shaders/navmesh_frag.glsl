#version 410

in vec4 world_pos;
in vec4 world_normal;

layout( location = 0 ) out vec4 frag_colour;

void main() {

//	vec3 colour = vec3(1);
	vec3 colour = vec3(abs(world_pos.x) * 0.05f, abs(world_pos.y) * 0.07f, abs(world_pos.z) * 0.1f);

	// grid every 1-unit
	if (mod( world_pos.x, 1.0 ) < 0.05f ||
		mod( world_pos.y, 1.0 ) < 0.05f ||
		mod( world_pos.z, 1.0 ) < 0.05f)
		colour = vec3(0);

	// fake light
	float d = max( 0, dot( normalize(vec3(1,1,1)),
			normalize(world_normal.xyz) ) ) * 0.75f;
	frag_colour.rgb = colour * 0.25f + colour * d;

	frag_colour.a = 1;
}
