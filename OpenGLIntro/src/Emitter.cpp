#include "Emitter.h"
#include "gl_core_4_4.h"

Emitter::Emitter() :
m_aParticles(nullptr), m_uiMaxParticles(0), m_uiAliveCount(0), m_VertexData(nullptr), m_uiIndexData(nullptr),
m_position(0), m_fEmitRate(0), m_fEmitTimer(0), m_fLifespanMin(0), m_fLifespanMax(0),
m_fVelocityMin(0), m_fVelocityMax(0), m_fStartSize(0), m_fEndSize(0),
m_startColour(0), m_endColour(0), m_fPrintTimer(0), m_fEmitRateMultiplier(0.02)
{

}

Emitter::~Emitter()
{
	delete[]	m_aParticles;
	delete[]	m_VertexData;
	delete[]	m_uiIndexData;

	glDeleteVertexArrays(1, &m_Buffers.m_uiVAO);
	glDeleteBuffers(1, &m_Buffers.m_uiVBO);
	glDeleteBuffers(1, &m_Buffers.m_uiIBO);
}

void	Emitter::Init(unsigned int a_MaxParticles, vec3 a_position, float a_fEmitRate,
	float a_fMinLifespan, float a_fMaxLifespan,
	float a_fMinVelocity, float a_fMaxVelocity,
	float a_fGravityStrength,
	float a_fStartSize, float a_fEndSize,
	vec4 a_StartColour, vec4 a_EndColour)
{
	m_position = vec4(a_position, 1);
	m_fLifespanMin = a_fMinLifespan;
	m_fLifespanMax = a_fMaxLifespan;
	m_fVelocityMin = a_fMinVelocity;
	m_fVelocityMax = a_fMaxVelocity;
	m_fGravityStrength = a_fGravityStrength;
	m_fStartSize = a_fStartSize;
	m_fEndSize = a_fEndSize;
	m_startColour = a_StartColour;
	m_endColour = a_EndColour;
	m_fEmitRate = a_fEmitRate;
	m_fPrintTimer = 0.0f;

	m_uiMaxParticles = a_MaxParticles;
	m_uiAliveCount = 0;
	m_aParticles = new Particle[m_uiMaxParticles];
	m_VertexData = new VertexParticle[m_uiMaxParticles * 4];

	m_uiIndexData = new unsigned int[m_uiMaxParticles * 6];

	for (unsigned int i = 0; i < m_uiMaxParticles; ++i)
	{
		//	0, 1, 2   0, 2, 3      4, 5, 6   4, 6, 7
		unsigned int start = 4 * i;
		m_uiIndexData[i * 6 + 0] = start + 0;
		m_uiIndexData[i * 6 + 1] = start + 1;
		m_uiIndexData[i * 6 + 2] = start + 2;

		m_uiIndexData[i * 6 + 3] = start + 0;
		m_uiIndexData[i * 6 + 4] = start + 2;
		m_uiIndexData[i * 6 + 5] = start + 3;
	}

	glGenVertexArrays(1, &m_Buffers.m_uiVAO);
	glGenBuffers(1, &m_Buffers.m_uiVBO);
	glGenBuffers(1, &m_Buffers.m_uiIBO);

	glBindVertexArray(m_Buffers.m_uiVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers.m_uiVBO);
	glBufferData(GL_ARRAY_BUFFER, m_uiMaxParticles * 4 * sizeof(VertexParticle), m_VertexData, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers.m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_uiMaxParticles * 6 * sizeof(unsigned int), m_uiIndexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	colour

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexParticle), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexParticle), (void*)sizeof(vec4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	Emitter::EmitParticles()
{
	unsigned int	uiParticlesToEmit = (unsigned int)(m_fEmitTimer * m_fEmitRate);
	m_fEmitTimer -= uiParticlesToEmit / m_fEmitRate;

	for (unsigned int i = 0; (i < uiParticlesToEmit) && (m_uiAliveCount < m_uiMaxParticles); ++i, ++m_uiAliveCount)
	{
		m_aParticles[m_uiAliveCount].position = m_position;
		m_aParticles[m_uiAliveCount].lifetime = 0;
		m_aParticles[m_uiAliveCount].lifespan = glm::linearRand(m_fLifespanMin, m_fLifespanMax);
		m_aParticles[m_uiAliveCount].colour = m_startColour;
		m_aParticles[m_uiAliveCount].size = m_fStartSize;
		float velocityLength = glm::linearRand(m_fVelocityMin, m_fVelocityMax);
		m_aParticles[m_uiAliveCount].velocity.xyz = glm::sphericalRand(velocityLength);
		m_aParticles[m_uiAliveCount].velocity.w = 0;
	}
}

void	Emitter::Update(float a_fDt)
{
	//	clean up dead particles
	for (unsigned int i = 0; i < m_uiAliveCount; ++i)
	{
		m_aParticles[i].lifetime += a_fDt;
		if (m_aParticles[i].lifetime > m_aParticles[i].lifespan)
		{
			--m_uiAliveCount;
			m_aParticles[i] = m_aParticles[m_uiAliveCount];
			--i;
		}
	}

	m_fEmitTimer += a_fDt;
	EmitParticles();

	///////////////////////////////// debug info for FPS and particle count.
	m_fPrintTimer += a_fDt;
	if (m_fPrintTimer > 1.0f)
	{
		printf("FPS: %f.  Particles: %d.  Emit Rate: %.3f.\n", 1/a_fDt, m_uiAliveCount, m_fEmitRate);
		m_fPrintTimer = 0.0f;
	}

	//	move all the alive particles
	for (unsigned int i = 0; i < m_uiAliveCount; ++i)
	{
		//	add gravity
		m_aParticles[i].velocity.y -= a_fDt * m_fGravityStrength;
		m_aParticles[i].position += a_fDt * m_aParticles[i].velocity;
		float	t = m_aParticles[i].lifetime / m_aParticles[i].lifespan;

		m_aParticles[i].colour = glm::mix(m_startColour, m_endColour, t);
		m_aParticles[i].size = glm::mix(m_fStartSize, m_fEndSize, t);
	}
}

void	Emitter::UpdateVertexData(vec3 camPos, vec3 camUp)
{
	for (unsigned int i = 0; i < m_uiAliveCount; ++i)
	{
		unsigned int iTimes4 = i * 4;
		mat4	particleTransform;

		vec3	to = camPos - m_aParticles[i].position.xyz;

		vec3	f = glm::normalize(to);
		vec3	r = glm::cross(camUp, f);
		vec3	u = glm::cross(f, r);

		f *= m_aParticles[i].size;
		r *= m_aParticles[i].size;
		u *= m_aParticles[i].size;

		particleTransform[0].xyz = r;
		particleTransform[1].xyz = u;
		particleTransform[2].xyz = f;
		particleTransform[3] = m_aParticles[i].position;

		m_VertexData[iTimes4].position = particleTransform * vec4(-1, 1, 0, 1);
		m_VertexData[iTimes4 + 1].position = particleTransform * vec4(-1, -1, 0, 1);
		m_VertexData[iTimes4 + 2].position = particleTransform * vec4(1, -1, 0, 1);
		m_VertexData[iTimes4 + 3].position = particleTransform * vec4(1, 1, 0, 1);

		m_VertexData[iTimes4].colour = m_aParticles[i].colour;
		m_VertexData[iTimes4 + 1].colour = m_aParticles[i].colour;
		m_VertexData[iTimes4 + 2].colour = m_aParticles[i].colour;
		m_VertexData[iTimes4 + 3].colour = m_aParticles[i].colour;
	}
}

void	Emitter::Render()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers.m_uiVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_uiAliveCount * 4 * sizeof(VertexParticle), m_VertexData);
	glBindVertexArray(m_Buffers.m_uiVAO);
	glDrawElements(GL_TRIANGLES, 6 * m_uiAliveCount, GL_UNSIGNED_INT, 0);
}

void	Emitter::IncreaseEmitRate()
{
	m_fEmitRate *= (1.0 + m_fEmitRateMultiplier);
}

void	Emitter::DecreaseEmitRate()
{
	m_fEmitRate /= 1.0 + (m_fEmitRateMultiplier * 5);
}


