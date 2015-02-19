#ifndef _QUATERNIONS_H_
#define _QUATERNIONS_H_

#include "Application.h"
#include "Camera.h"

struct KeyFrame
{
	vec3	position;
	glm::quat	rotation;
};

class Quaternions : public Application
{
public:
	Quaternions();
	virtual	~Quaternions();

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

	glm::vec3	m_positions[2];
	glm::quat	m_rotations[2];

	mat4	hip_bone;
	mat4	knee_bone;
	mat4	ankle_bone;

	KeyFrame	m_HipFrames[2];
	KeyFrame	m_KneeFrames[2];
	KeyFrame	m_AnkleFrames[2];
};

#endif	//	_QUATERNIONS_H_
