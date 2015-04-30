#ifndef _GAME_TREE_SEARCH_H_
#define _GAME_TREE_SEARCH_H_

#include "Application.h"
#include "Camera.h"

#include "AntTweakBar.h"
#include "ConnectFour.h"

class GameTreeSearch : public Application
{
public:
	GameTreeSearch();
	virtual	~GameTreeSearch();

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

	//	the game and AI components
	Game*	m_game;
	AI*		m_ai;

	vec3	m_pickPosition;
};

#endif	//	_GAME_TREE_SEARCH_H_
