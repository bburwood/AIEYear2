#ifndef _BLANK_H_
#define _BLANK_H_

#include "Application.h"
#include "Camera.h"

#include "AntTweakBar.h"

class BLANK : public Application
{
public:
	BLANK();
	virtual	~BLANK();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

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

#endif	//	_BLANK_H_
