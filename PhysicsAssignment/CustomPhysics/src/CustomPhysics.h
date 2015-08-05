#ifndef _CUSTOMPHYSICS_H_
#define _CUSTOMPHYSICS_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"
#include "AntTweakBar.h"
#include "DIYPhysicsEngine.h"

#include <vector>

//const	int	c_iNUM_EMITTERS = 64;

class CustomPhysics : public Application
{
public:
	CustomPhysics();
	~CustomPhysics();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	void	SetupPoolGame();
	void	CreateChain(glm::vec2 a_vStartPos, glm::vec2 a_vEndPos, int a_iSegments, float a_fSpringK, float a_fSpringDamping);
	void	UpdatePhysics(float delta);
	glm::vec2	GetWorldMouse();

	void	ReloadShader();
	void	BuildBasicCube(OpenGLData& a_cube, float a_fSideLength);
	void	BuildBackboard(OpenGLData& a_board, float a_fSideLength, float a_fThickness);	//	change name to BuildTexturedCube
	void	LoadMeshes();
	void	DrawModels(vec3 a_vLightDir, vec4 a_vLightColour);
	//void	DrawSelectedBox(int a_iXIndex, int a_iZIndex, float a_fSideLength, float a_fYCoord, vec4 a_Colour);


	mat4	m_view;
	mat4	m_projection;
	float	m_fTimer;
	float	m_fTotalTime;
	float	m_fPhysicsUpdateTimer;
	float	m_fPhysicsUpdateTimout;
	float	m_fPhysicsFrameRate;
	float	m_fCueSpeed;
	float	m_fGravityStrength;

	DIYPhysicScene* physicsScene;

	FlyCamera	m_FlyCamera;
	vec4	m_BackgroundColour;
	vec4	m_vAmbientLightColour;
	//	vec4	m_;
	float	m_fFPS;

	TwBar*	m_bar;

	bool	m_bDrawGizmos;
	bool	m_bDrawGrid;
	bool	m_bDebug;
	bool	m_bGravity;

private:

};

#endif	//	_CUSTOMPHYSICS_H_
