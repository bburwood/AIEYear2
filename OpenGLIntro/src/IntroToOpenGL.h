#ifndef _INTRO_TO_OPENGL_H_
#define _INTRO_TO_OPENGL_H_

#include "Application.h"

const	int	PLANET_COUNT = 5;

class IntroToOpenGL : public Application
{
public:
	IntroToOpenGL();
	virtual	~IntroToOpenGL();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;

};

#endif	//	_INTRO_TO_OPENGL_H_
