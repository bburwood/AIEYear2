#ifndef _CHECKERS_H_
#define _CHECKERS_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"
#include "GPUEmitter.h"
#include "AntTweakBar.h"

#include "Game.h"

const	int	c_iNUM_EMITTERS = 64;


class Checkers : public Application
{
public:
	Checkers();
	~Checkers();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	void	ReloadShader();
	void	BuildBasicCube(OpenGLData& a_cube, float a_fSideLength);
	void	BuildCheckerboard(OpenGLData& a_board, float a_fSideLength, float a_fThickness);
	void	BuildBackboard(OpenGLData& a_board, float a_fSideLength, float a_fThickness);
	void	LoadMeshes();
	void	DrawModels();
	void	DrawSelectedBox(int a_iXIndex, int a_iZIndex, float a_fYCoord, vec4 a_Colour);

	OpenGLData	m_BoardMesh;
	OpenGLData	m_BackBoardMesh;
	OpenGLData	m_CheckerMesh;
	OpenGLData	m_KingMesh;

	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;
	float	m_fTotalTime;
	unsigned int	m_uiProgramID;	//	shader for the checkerboard and backboard
	unsigned int	m_uiModelProgramID;	//	shader for the checkers themselves
	unsigned int	m_uiParticleTexture;
	unsigned int	m_uiCheckerBoardTexture;
	unsigned int	m_uiBackBoardTexture;
	FlyCamera	m_FlyCamera;
	vec4	m_BackgroundColour;
	vec4	m_Player1Colour;
	vec4	m_Player2Colour;
	vec4	m_vAmbientLightColour;
	vec4	m_vLightColour;
	vec3	m_vLightDir;
//	vec4	m_;
	float	m_fFPS;

	TwBar*	m_bar;

	bool	m_bDrawGizmos;
	bool	m_bResetGame;


private:
	Game	m_Game;
	GPUPointEmitter	m_emitters[c_iNUM_EMITTERS];	//	have 4 gpu particle emitters, used for "exploding" pieces when captured
	vec4	m_CurrentPlayerColour;

	float	m_fFiringTimer;
	float	m_fFiringInterval;
	float	m_fEmitterLifespan;
	float	m_fEmitterParticleLifespan;
	unsigned int		m_uiEmitterMaxParticles;
	float	m_fEmitRate;
	int		m_iNextEmitterToFire;

	float	m_fCheckerboardSpecPower;
	float	m_fCheckerPieceSpecPower;

	mat4	m_CheckerBoardWorldTransform;
	mat4	m_BackBoardWorldTransform;
	mat4	m_CheckerWorldTransform;
	mat4	m_KingWorldTransform;

	int		m_iPlayerToMoveFirst;

	int		m_iXSelected;
	int		m_iZSelected;
	bool	m_bPieceSelected;

};

#endif	//	_CHECKERS_H_
