#ifndef _CAMERA_AND_PROJECTIONS_H_
#define _CAMERA_AND_PROJECTIONS_H_

#include "Application.h"
#include "Camera.h"

class CameraAndProjections : public Application
{
public:
	CameraAndProjections();
	virtual	~CameraAndProjections();

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
};

#endif	//	_CAMERA_AND_PROJECTIONS_H_
