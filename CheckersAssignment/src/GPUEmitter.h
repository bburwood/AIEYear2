#ifndef _GPUEMITTER_H_
#define _GPUEMITTER_H_

#include "GLMHeader.h"
#include "Vertex.h"

struct GPUParticle
{
	GPUParticle() : lifespan(0), lifetime(1) {}
	vec3	position;
	vec3	velocity;
	float	lifetime;
	float	lifespan;
};

//	What does an Emitter do?
//	It contains all of the particles that it controls
//	It spawns new particles
//	It updates existing particles
//	It renders its particles


class GPUPointEmitter
{
public:
	GPUPointEmitter();
	~GPUPointEmitter();

	void	Init(unsigned int a_MaxParticles, vec3 a_position, vec3 a_velocity,
		float a_fEmitRate, float a_fEmitterLifespan,
		float a_fMinLifespan, float a_fMaxLifespan,
		float a_fMinVelocity, float a_fMaxVelocity,
		float a_fGravityStrength,
		float a_fStartSize, float a_fEndSize,
		vec4 a_StartColour, vec4 a_EndColour, int a_iEmitterID);
	void	Draw(float a_currTime, mat4 a_cameraTransform, mat4 a_projectionView, unsigned int a_uiFBO = 0xFFFFFFFF, bool a_bDeferred = false);
	void	CreateBuffers();
	void	CreateUpdateShader();
	void	CreateDrawShader();
	void	SetParticleTexture(unsigned int a_uiParticleTexture);



/*
	void	EmitParticles();
	void	IncreaseEmitRate();
	void	DecreaseEmitRate();
	void	Update(float a_fDt);
	void	UpdateVertexData(vec3 camPos, vec3 camUp);
	void	Render();
*/

	//	Particle Data
	GPUParticle*	m_aParticles;
	unsigned int	m_uiMaxParticles;
	unsigned int	m_uiAliveCount;

	//	OpenGL Data
//	OpenGLData	m_Buffers;
//	VertexParticle*	m_VertexData;
//	unsigned int*	m_uiIndexData;

	//	Emitter Data
	vec3	m_position;

	float	m_fEmitRate;
	float	m_fEmitTimer;

	float	m_fLifespanMin;
	float	m_fLifespanMax;

	float	m_fVelocityMin;
	float	m_fVelocityMax;
	float	m_fGravityStrength;

	float	m_fStartSize;
	float	m_fEndSize;

	vec4	m_startColour;
	vec4	m_endColour;

	unsigned int	m_uiActiveBuffer;
	unsigned int	m_VAO[2];
	unsigned int	m_VBO[2];
	unsigned int	m_uiUpdateShader;
	unsigned int	m_uiDrawShader;
	//float	m_fLastDrawTime;


	//	General data
	float	m_fPrintTimer;
	float	m_fEmitRateMultiplier;

private:
	vec3	m_vVelocity;
	vec3	m_vCamLastForward;
	float	m_fEmitterLifespan;
	float	m_fCurrentAge;
	int		m_iEmitterID;
	//	the simple particle texture was obtained from: http://buildnewgames.com/particle-systems/
	//	it is a 32bit ARGB png texture, 128x128
	unsigned int	m_uiParticleTexture;
	bool	m_bDeleteBuffers = false;
};

#endif	//	_GPUEMITTER_H_
