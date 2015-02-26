#ifndef _PARTICLES_H_
#define _PARTICLES_H_

#include "Application.h"
#include "Camera.h"
#include "Emitter.h"

class Particles : public Application
{
public:
	Particles();
	virtual	~Particles();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;
	FlyCamera	m_FlyCamera;
	vec4	m_BackgroundColour;
	bool	m_bDrawGizmos;
	Emitter	m_emitter;
	unsigned int	m_uiProgramID;
};

#endif	//	_PARTICLES_H_
