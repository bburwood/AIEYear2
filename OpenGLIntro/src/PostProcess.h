#ifndef _POST_PROCESS_H_
#define _POST_PROCESS_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

#include "AntTweakBar.h"

class PostProcess : public Application
{
public:
	PostProcess();
	virtual	~PostProcess();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	void	GenerateFrameBuffer();
	void	GenerateScreenSpaceQuad();

	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;
	unsigned int	m_uiProgramID;
	unsigned int	m_uiPostProgramID;
	unsigned int	m_uifbo;
	unsigned int	m_uifboTexture;
	unsigned int	m_uifboDepth;
	FlyCamera	m_FlyCamera;
	vec4	m_BackgroundColour;
	float	m_fFPS;

	TwBar*	m_bar;

	OpenGLData	m_quad;

	bool	m_bDrawGizmos;
};

#endif	//	_POST_PROCESS_H_
