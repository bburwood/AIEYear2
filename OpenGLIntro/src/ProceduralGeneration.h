#ifndef _PROCEDURAL_GENERATION_H_
#define _PROCEDURAL_GENERATION_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

#include "AntTweakBar.h"

class ProceduralGeneration : public Application
{
public:
	ProceduralGeneration();
	virtual	~ProceduralGeneration();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	void	BuildGrid(vec2 a_RealDims, glm::ivec2 a_Dims);
	void	BuildPerlinTexture(glm::ivec2 a_Dims, unsigned int a_uiOctaves, float a_fPersistence);

	OpenGLData	m_PlaneMesh;
	unsigned int	m_uiPerlinTexture;
	float*	m_fPerlinData;
	float	m_fScale;
	int		m_uiOctaves;
	vec2	m_GridDimensions;
	float	m_fRealWidth;
	float	m_fRealHeight;
	float	m_fHighest;	//	used to store the current highest Perlin value
	float	m_fLowest;	//	used to store the current lowest Perlin value
	glm::ivec2	m_MeshDimensions;
	unsigned int		m_iMeshWidth;
	unsigned int		m_iMeshHeight;


	void	ReloadShader();

	

	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;
	unsigned int	m_uiProgramID;
	FlyCamera	m_FlyCamera;
	vec4	m_BackgroundColour;
	float	m_fFPS;

	TwBar*	m_bar;

	bool	m_bDrawGizmos;
};

#endif	//	_PROCEDURAL_GENERATION_H_
