#ifndef _CHECKERS_H_
#define _CHECKERS_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"
#include "GPUEmitter.h"
#include "AntTweakBar.h"

#include "Game.h"
#include <vector>

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
	void	DrawModels(vec3 a_vLightDir, vec4 a_vLightColour);
	void	DrawSelectedBox(int a_iXIndex, int a_iZIndex, float a_fSideLength, float a_fYCoord, vec4 a_Colour);
	void	FireEmitterAt(int a_iXIndex, int a_iZindex, float a_fHeight);
	void	FireCaptureEmitterAt(int a_iXIndex, int a_iZindex, float a_fHeight);
	void	FireGameOverEmitterAt(int a_iXIndex, int a_iZindex, float a_fHeight, int a_iWinner);
	void	DrawBitboardAsBoxes(Bitboard a_bbBoard, float a_fBoxSize, vec4 a_vColour);
	void	DebugDrawMoveList(float a_fHeight, vec4 a_vColour);

	//	Deferred Lighting additions ...
	void	SetupDeferredLights();
	void	AddPointLight(float a_fX, float a_fY, float a_fZ, float a_fR, float a_fG, float a_fB, float a_fRange);//
	void	RenderPointLights();//
	void	RenderDirectionalLight(vec3 light_dir, vec3 light_colour);
	void	BuildScreenSpaceQuad();//
	void	BuildLightCube();//
	void	BuildGBuffer();//
	void	BuildLightBuffer();//
	void	DrawModelsDeferred();
	void	AccumulateLightsDeferred();
	void	RenderCompositePass();

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

	//	public debug stuff ...
	Bitboard	bbDAvailableMovers;

	TwBar*	m_bar;

	bool	m_bDrawGizmos;
	bool	m_bResetGame;
	bool	m_bDebug;
	bool	m_bDeferredRendering;


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
	unsigned int	m_uiGBufferTexturedProgram;
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

#endif	//	_CHECKERS_H_
