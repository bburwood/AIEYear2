#version 410

layout (location=0) in vec4 position;
layout (location=1) in vec4 normal;
layout (location=2) in vec4 tangent;
layout (location=3) in vec2 tex_coord;

out vec3 frag_normal;
out vec3 frag_position;
out vec3 frag_tangent;
out vec3 frag_bitangent;
out vec2 frag_texcoord;

uniform mat4 projection_view;

void main()
{
	frag_normal = normal.xyz;
	frag_position = position.xyz;
	frag_tangent = tangent.xyz;
	frag_bitangent = cross(normal.xyz, tangent.xyz);
	frag_texcoord = tex_coord;
	gl_Position = projection_view * position;
}
