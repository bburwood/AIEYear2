#ifndef _PHYSICS_APP_H_
#define _PHYSICS_APP_H_

#include "Application.h"
#include "Camera.h"
#include "BBPhysics.h"

#include "AntTweakBar.h"

class PhysicsApp : public Application
{
public:
	PhysicsApp();
	virtual	~PhysicsApp();

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

	BBPhysics	m_oBBPhysics;

	TwBar*	m_bar;

	bool	m_bDrawGizmos;
};

#endif	//	_PHYSICS_APP_H_
