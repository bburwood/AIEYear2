#ifndef _CUSTOMPHYSICS_H_
#define _CUSTOMPHYSICS_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"
#include "AntTweakBar.h"

#include <vector>

const	int	c_iNUM_EMITTERS = 64;


class CustomPhysics : public Application
{
public:
	CustomPhysics();
	~CustomPhysics();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	void	ReloadShader();
	void	BuildBasicCube(OpenGLData& a_cube, float a_fSideLength);
	void	BuildBackboard(OpenGLData& a_board, float a_fSideLength, float a_fThickness);	//	change name to BuildTexturedCube
	void	LoadMeshes();
	void	DrawModels(vec3 a_vLightDir, vec4 a_vLightColour);
	//void	DrawSelectedBox(int a_iXIndex, int a_iZIndex, float a_fSideLength, float a_fYCoord, vec4 a_Colour);

	OpenGLData	m_BoardMesh;
	OpenGLData	m_BackBoardMesh;
	OpenGLData	m_CheckerMesh;
	OpenGLData	m_KingMesh;
	//	deferred rendering extras
	OpenGLData	m_ScreenSpaceQuad;
	OpenGLData	m_LightCube;

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
	bool	m_bDebug;
	bool	m_bDeferredRendering;


private:
	float	m_fFiringTimer;
	float	m_fFiringInterval;
	float	m_fEmitterLifespan;
	float	m_fEmitterParticleLifespan;
	unsigned int		m_uiEmitterMaxParticles;
	float	m_fEmitRate;
	int		m_iNextEmitterToFire;

	float	m_fCheckerboardSpecPower;
	float	m_fCheckerPieceSpecPower;
	float	m_fDeferredSpecPower;

	mat4	m_CheckerBoardWorldTransform;
	mat4	m_BackBoardWorldTransform;
	mat4	m_CheckerWorldTransform;
	mat4	m_KingWorldTransform;

	int		m_iPlayerToMoveFirst;

	//	Deferred Lighting additions ...
	std::vector<float>	aPointLightsX;
	std::vector<float>	aPointLightsY;
	std::vector<float>	aPointLightsZ;
	std::vector<float>	aPointLightsColourR;
	std::vector<float>	aPointLightsColourG;
	std::vector<float>	aPointLightsColourB;
	std::vector<float>	aPointLightsRange;
	//	we need our shaders
	unsigned int	m_uiGBufferProgram;
	unsigned int	m_uiDirectionalLightProgram;
	unsigned int	m_uiPointLightProgram;
	unsigned int	m_uiCompositeProgram;
	//	generate a g-buffer
	unsigned int	m_uiGBufferFBO;
	unsigned int	m_uiAlbedoTexture;
	unsigned int	m_uiNormalsTexture;
	unsigned int	m_uiPositionsTexture;
	unsigned int	m_uiGBufferDepth;
	//	render lights
	unsigned int	m_uiLightFBO;
	unsigned int	m_uiLightTexture;


	int		m_iXSelected;
	int		m_iZSelected;
	bool	m_bPieceSelected;

};

#endif	//	_CUSTOMPHYSICS_H_
