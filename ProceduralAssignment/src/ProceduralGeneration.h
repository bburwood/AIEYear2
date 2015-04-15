#ifndef _PROCEDURAL_GENERATION_H_
#define _PROCEDURAL_GENERATION_H_

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"
#include "GPUEmitter.h"

#include "AntTweakBar.h"

const	int	c_iNUM_EMITTERS = 4;

class ProceduralGeneration : public Application
{
public:
	ProceduralGeneration();
	virtual	~ProceduralGeneration();

	virtual	bool	startup();
	virtual	void	shutdown();
	virtual	bool	update();
	virtual	void	draw();

	void	BuildGrid(vec2 a_RealDims, glm::ivec2 a_Dims);
	void	BuildPerlinTexture(glm::ivec2 a_Dims, unsigned int a_uiOctaves, float a_fPersistence);
	void	DrawModels();
	//void	LoadPlaneMesh();

	OpenGLData	m_PlaneMesh;
	OpenGLData	m_F16Mesh;
	OpenGLData	m_F16CopyMesh;
	unsigned int	m_uiPerlinTexture;
	unsigned int	m_uiWaterTexture;
	unsigned int	m_uiGrassTexture;
	unsigned int	m_uiSnowTexture;
	unsigned int	m_uiParticleTexture;
	unsigned int	m_uiF16Texture;
	float*	m_fPerlinData;
	float	m_fPerlinScale;
	float	m_fTerrainHeight;
	int		m_uiOctaves;
	vec2	m_GridDimensions;
	float	m_fRealWidth;
	float	m_fRealHeight;
	glm::ivec2	m_MeshDimensions;
	unsigned int		m_iMeshWidth;
	unsigned int		m_iMeshHeight;


	void	ReloadShader();



	mat4	m_view;
	mat4	m_projection;
	float	m_camera_x;
	float	m_camera_z;
	float	m_timer;
	float	m_fTotalTime;
	unsigned int	m_uiProgramID;
	unsigned int	m_uiModelProgramID;
	FlyCamera	m_FlyCamera;
	vec4	m_BackgroundColour;
	vec3	m_vLightDir;
	float	m_fFPS;

	TwBar*	m_bar;

	bool	m_bDrawGizmos;
private:
	float	m_fHighest;	//	used to store the current highest Perlin value
	float	m_fLowest;	//	used to store the current lowest Perlin value
	float	m_fHighestX;
	float	m_fHighestZ;
	float	m_fLowestX;
	float	m_fLowestZ;
	GPUPointEmitter	m_emitters[c_iNUM_EMITTERS];	//	have 4 gpu particle emitters.  The idea is to eventually have them spaced along
	//	the F16's wings and fire them one by one.
	float	m_fFiringTimer;
	float	m_fFiringInterval;
	float	m_fEmitterLifespan;
	float	m_fEmitterParticleLifespan;
	unsigned int		m_uiEmitterMaxParticles;
	float	m_fEmitRate;
	int		m_iNextEmitterToFire;
	mat4	m_F16Transform;
	mat4	m_F16CopyTransform;
};

#endif	//	_PROCEDURAL_GENERATION_H_
