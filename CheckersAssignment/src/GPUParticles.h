#ifndef _GPUPARTICLES_H_
#define _GPUPARTICLES_H_

#include "Application.h"
#include "Camera.h"
#include "GPUEmitter.h"

class GPUParticles : public Application
{
public:
	GPUParticles();
	virtual	~GPUParticles();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	GPUPointEmitter	m_emitter;

	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;
	FlyCamera	m_FlyCamera;
	vec4	m_BackgroundColour;
	bool	m_bDrawGizmos;
};

#endif	//	_GPUPARTICLES_H_
