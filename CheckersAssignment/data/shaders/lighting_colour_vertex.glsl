#version 410

layout (location=0) in vec4 position;
layout (location=1) in vec4 normal;
//layout (location=2) in vec2 texCoord;


out vec4 frag_position;
out vec4 frag_normal;
//out vec2 lfragTexCoord;

uniform mat4 projection_view;
uniform mat4 worldTransform;

void main()
{
	frag_normal = normal;
	frag_position = worldTransform * position;
//	lfragTexCoord = texCoord;
	gl_Position = projection_view * worldTransform * position;
}
