#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "GLMHeader.h"

struct Vertex
{
	vec4	position;
	vec4	colour;
};

struct VertexTexCoord
{
	vec4	position;
	vec2	tex_coord;
};

struct VertexNormal
{
	vec4	position;
	vec4	normal;
	vec4	tangent;
	vec2	tex_coord;
};

struct OpenGLData
{
	unsigned int	m_uiVAO;
	unsigned int	m_uiVBO;
	unsigned int	m_uiIBO;
	unsigned int	m_uiIndexCount;
};

struct VertexParticle
{
	vec4	position;
	vec4	colour;
};


#endif	//	_VERTEX_H_
