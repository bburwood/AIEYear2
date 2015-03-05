#ifndef _SCENEMANAGEMENT_H_
#define _SCENEMANAGEMENT_H_

#include "Application.h"
#include "Camera.h"

#include "AntTweakBar.h"

class SceneManagement : public Application
{
public:
	SceneManagement();
	virtual	~SceneManagement();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	void	getFrustumPlanes(const glm::mat4& transform, glm::vec4* planes);

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

#endif	//	_SCENEMANAGEMENT_H_
