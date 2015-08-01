#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 textureCoords;

uniform mat4 projectionView;

void main()
{
	textureCoords = texCoords;
	gl_Position = projectionView * vec4(position, 1);
}
