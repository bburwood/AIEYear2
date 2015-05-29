#ifndef VERTEX_H_
#define VERTEX_H_

#include "glm\glm.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

struct Vertex
{
	vec4 position;
	vec4 color;
};

struct VertexTexCoord
{
	vec4 position;
	vec2 tex_coord;
};

struct VertexNormal
{
	vec4 position;
	vec4 normal;
	vec4 tangent ;
	vec2 tex_coord;
};

struct VertexParticle
{
	vec4 position;
	vec4 color;
};

struct OpenGLData
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	unsigned int m_index_count;
};

#endif //VERTEX_H_