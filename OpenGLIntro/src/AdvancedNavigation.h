#ifndef _ADVANCED_NAVIGATION_H_
#define _ADVANCED_NAVIGATION_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"
#include "AntTweakBar.h"
#include <vector>

struct NavMeshNode
{
	vec3	position;	//	centre point of the polygon
	vec3	corners[3];
	NavMeshNode*	edges[3];

	//	A* data
	unsigned int	uiFlags;
	float	edge_costs[3];
};

class AdvancedNavigation : public Application
{
public:
	AdvancedNavigation();
	virtual	~AdvancedNavigation();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	void	ReloadShader();
	void	BuildNavMesh();

	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;
	unsigned int	m_uiProgramID;
	OpenGLData	m_SponzaMesh;
	std::vector<NavMeshNode>	m_aNavNodes;

	FlyCamera	m_FlyCamera;
	vec4	m_BackgroundColour;
	float	m_fFPS;

	TwBar*	m_bar;
	bool	m_bDrawGizmos;
};

#endif	//	_ADVANCED_NAVIGATION_H_
