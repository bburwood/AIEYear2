#ifndef _SHADOWS_H_
#define _SHADOWS_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

#include "AntTweakBar.h"

class Shadows : public Application
{
public:
	Shadows();
	virtual	~Shadows();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	void	ReloadShader();

	void	BuildMeshes();
	void	BuildShadowMap();

	OpenGLData	m_Bunny;
	OpenGLData	m_plane;

	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;
	unsigned int	m_uiProgramID;
	unsigned int	m_uiShadowMapProgramID;
	unsigned int	m_uiFBO;
	unsigned int	m_uiFBODepth;
	unsigned int	m_uiDiffuseProgramID;

	vec3	m_LightDirection;
	mat4	m_LightMatrix;

	FlyCamera	m_FlyCamera;
	vec4	m_BackgroundColour;
	float	m_fFPS;

	TwBar*	m_bar;

	bool	m_bDrawGizmos;
};

#endif	//	_SHADOWS_H_
