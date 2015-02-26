#ifndef _EMITTER_H_
#define _EMITTER_H_

#include "GLMHeader.h"
#include "Vertex.h"

struct Particle
{
	vec4	position;
	vec4	velocity;
	vec4	colour;
	float	size;
	float	lifetime;
	float	lifespan;
};

//	What does an Emitter do?
//	It contains all of the particles that it controls
//	It spawns new particles
//	It updates existing particles
//	It renders its particles



class Emitter
{
public:
	Emitter();
	~Emitter();

	void	Init(unsigned int a_MaxParticles, vec3 a_position, float a_fEmitRate,
			float a_fMinLifespan, float a_fMaxLifespan,
			float a_fMinVelocity, float a_fMaxVelocity,
			float a_fGravityStrength,
			float a_fStartSize, float a_fEndSize,
			vec4 a_StartColour, vec4 a_EndColour);

	void	EmitParticles();
	void	IncreaseEmitRate();
	void	DecreaseEmitRate();
	void	Update(float a_fDt);
	void	UpdateVertexData(vec3 camPos, vec3 camUp);
	void	Render();


	//	Particle Data
	Particle*	m_aParticles;
	unsigned int	m_uiMaxParticles;
	unsigned int	m_uiAliveCount;

	//	OpenGL Data
	OpenGLData	m_Buffers;
	VertexParticle*	m_VertexData;
	unsigned int*	m_uiIndexData;

	//	Emitter Data
	vec4	m_position;

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

	//	General data
	float	m_fPrintTimer;
	float	m_fEmitRateMultiplier;

private:

};

#endif	//	_EMITTER_H_
