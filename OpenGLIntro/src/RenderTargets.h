#ifndef _RENDER_TARGETS_H_
#define _RENDER_TARGETS_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

#include "AntTweakBar.h"

class RenderTargets : public Application
{
public:
	RenderTargets();
	virtual	~RenderTargets();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	void	GenerateFrameBuffer();
	void	GeneratePlane();

	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;
	unsigned int	m_uiProgramID;
	unsigned int	m_uifbo;
	unsigned int	m_uifboTexture;
	unsigned int	m_uifboDepth;

	OpenGLData	m_plane;	//	needs Vertex.h

	FlyCamera	m_FlyCamera;
	vec4	m_BackgroundColour;
	float	m_fFPS;

	TwBar*	m_bar;

	bool	m_bDrawGizmos;
};

#endif	//	_RENDER_TARGETS_H_
