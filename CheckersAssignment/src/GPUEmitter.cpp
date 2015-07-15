#include "GPUEmitter.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"

GPUPointEmitter::GPUPointEmitter() :
m_aParticles(nullptr), m_uiMaxParticles(0), m_uiAliveCount(0),
m_position(0), m_fEmitRate(0), m_fEmitTimer(0), m_fLifespanMin(0), m_fLifespanMax(0),
m_fVelocityMin(0), m_fVelocityMax(0), m_fStartSize(0), m_fEndSize(0),
m_startColour(0), m_endColour(0), m_fPrintTimer(0), m_fEmitRateMultiplier(0.02f),
m_uiDrawShader(0), m_uiUpdateShader(0), m_vVelocity(0), m_bDeleteBuffers(false)
{
	m_VAO[0] = 0;
	m_VAO[1] = 0;
	m_VBO[0] = 0;
	m_VBO[1] = 0;
}

GPUPointEmitter::~GPUPointEmitter()
{
	delete[]	m_aParticles;

	glDeleteVertexArrays(2, m_VAO);
	glDeleteBuffers(2, m_VAO);
	glDeleteBuffers(2, m_VBO);

	glDeleteProgram(m_uiUpdateShader);
	glDeleteProgram(m_uiDrawShader);
}

void	GPUPointEmitter::Init(unsigned int a_MaxParticles, vec3 a_position, vec3 a_velocity,
	float a_fEmitRate, float a_fEmitterLifespan,
	float a_fMinLifespan, float a_fMaxLifespan,
	float a_fMinVelocity, float a_fMaxVelocity,
	float a_fGravityStrength,
	float a_fStartSize, float a_fEndSize,
	vec4 a_StartColour, vec4 a_EndColour, int a_iEmitterID)
{
	m_position = a_position;
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

	if (m_aParticles != nullptr)
	{
		delete[]	m_aParticles;
		m_bDeleteBuffers = true;
	}
	m_aParticles = new GPUParticle[m_uiMaxParticles];
	m_uiActiveBuffer = 0;

	m_vVelocity = a_velocity;
	m_fEmitterLifespan = a_fEmitterLifespan;
	m_fCurrentAge = 0.0f;
	m_iEmitterID = a_iEmitterID;
	printf("Emitter %d:  Age: %1.2f  Particles: %d.\n", m_iEmitterID, m_fCurrentAge, m_uiMaxParticles);

	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();
}

void	GPUPointEmitter::Draw(float a_fDT, mat4 a_cameraTransform, mat4 a_projectionView, unsigned int a_uiFBO, bool a_bDeferred)
{
	///////////////////////////////// debug info for FPS and particle count.
	float fDt = a_fDT;	//	a_currTime - m_fLastDrawTime;
	m_fCurrentAge += fDt;
	if (m_fCurrentAge > (m_fEmitterLifespan + m_fLifespanMax))
	{
		//	then we have finished emitting for now
		return;
	}

	m_fPrintTimer += fDt;
	if (m_fPrintTimer > 1.0f)
	{
		//printf("Emitter %d:  Age: %1.2f  Lifespan: %1.2f  Particles: %d.\n", m_iEmitterID, m_fCurrentAge, m_fEmitterLifespan, m_uiMaxParticles);
		m_fPrintTimer = 0.0f;
	}

	glPointSize(100);
	m_position += (m_vVelocity * fDt);

	//	update vertex pass
	glUseProgram(m_uiUpdateShader);
	int	deltaUniform = glGetUniformLocation(m_uiUpdateShader, "delta_time");
	int	emitterPosUniform = glGetUniformLocation(m_uiUpdateShader, "emitter_position");
	int	minVelUniform = glGetUniformLocation(m_uiUpdateShader, "min_velocity");
	int	maxVelUniform = glGetUniformLocation(m_uiUpdateShader, "max_velocity");
	int	minLifespanUniform = glGetUniformLocation(m_uiUpdateShader, "min_lifespan");
	int	maxLifespanUniform = glGetUniformLocation(m_uiUpdateShader, "max_lifespan");
	int	gravityUniform = glGetUniformLocation(m_uiUpdateShader, "gravity_strength");
	int	iEmitterAgeUniform = glGetUniformLocation(m_uiUpdateShader, "fEmitterAge");
	int	iEmitterLifespanUniform = glGetUniformLocation(m_uiUpdateShader, "fEmitterLifespan");

	glUniform1f(deltaUniform, fDt);
	glUniform3fv(emitterPosUniform, 1, (float*)&m_position);
	glUniform1f(minVelUniform, m_fVelocityMin);
	glUniform1f(maxVelUniform, m_fVelocityMax);
	glUniform1f(minLifespanUniform, m_fLifespanMin);
	glUniform1f(maxLifespanUniform, m_fLifespanMax);
	glUniform1f(gravityUniform, m_fGravityStrength);
	glUniform1f(iEmitterAgeUniform, m_fCurrentAge);
	glUniform1f(iEmitterLifespanUniform, m_fEmitterLifespan);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_VAO[m_uiActiveBuffer]);

	unsigned int	uiOtherBuffer = (m_uiActiveBuffer + 1) % 2;
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_VBO[uiOtherBuffer]);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_uiMaxParticles);

	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK, 0, 0);

	//	render pass
	if (a_bDeferred)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, a_uiFBO);
	}
	glUseProgram(m_uiDrawShader);

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int	projViewUniform = glGetUniformLocation(m_uiDrawShader, "projection_view");
	int	cameraWorldUniform = glGetUniformLocation(m_uiDrawShader, "camera_world");
	int	startSizeUniform = glGetUniformLocation(m_uiDrawShader, "start_size");
	int	endSizeUniform = glGetUniformLocation(m_uiDrawShader, "end_size");
	int	startColourUniform = glGetUniformLocation(m_uiDrawShader, "start_colour");
	int	endColourUniform = glGetUniformLocation(m_uiDrawShader, "end_colour");
	iEmitterAgeUniform = glGetUniformLocation(m_uiDrawShader, "fEmitterAge");
	iEmitterLifespanUniform = glGetUniformLocation(m_uiDrawShader, "fEmitterLifespan");
	int	iEmitterAgePercentUniform = glGetUniformLocation(m_uiDrawShader, "fEmitterAgePercent");
	


	int iParticleTexUniform = glGetUniformLocation(m_uiDrawShader, "particleTexture");
	glUniform1i(iParticleTexUniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiParticleTexture);

	glUniformMatrix4fv(projViewUniform, 1, GL_FALSE, (float*)&a_projectionView);
	glUniformMatrix4fv(cameraWorldUniform, 1, GL_FALSE, (float*)&a_cameraTransform);
	glUniform1f(startSizeUniform, m_fStartSize);
	glUniform1f(endSizeUniform, m_fEndSize);
	glUniform4fv(startColourUniform, 1, (float*)&m_startColour);
	glUniform4fv(endColourUniform, 1, (float*)&m_endColour);
	glUniform1f(iEmitterAgeUniform, m_fCurrentAge);
	glUniform1f(iEmitterLifespanUniform, m_fEmitterLifespan);
	glUniform1f(iEmitterAgePercentUniform, m_fCurrentAge / m_fEmitterLifespan);

	glBindVertexArray(m_VAO[uiOtherBuffer]);
	glDrawArrays(GL_POINTS, 0, m_uiMaxParticles);
	m_uiActiveBuffer = uiOtherBuffer;
	//m_fLastDrawTime = a_currTime;
	glEnable(GL_DEPTH_TEST);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void	GPUPointEmitter::CreateBuffers()
{
	if (m_bDeleteBuffers)
	{
		glDeleteVertexArrays(2, m_VAO);
		glDeleteBuffers(2, m_VAO);
		glDeleteBuffers(2, m_VBO);
	}

	glGenVertexArrays(2, m_VAO);
	glGenBuffers(2, m_VBO);
	for (unsigned int uiBufferIndex = 0; uiBufferIndex < 2; ++uiBufferIndex)
	{
		glBindVertexArray(m_VAO[uiBufferIndex]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[uiBufferIndex]);
		glBufferData(GL_ARRAY_BUFFER, m_uiMaxParticles * sizeof(GPUParticle), m_aParticles, GL_STREAM_DRAW);

		glEnableVertexAttribArray(0);	//	position
		glEnableVertexAttribArray(1);	//	velocity
		glEnableVertexAttribArray(2);	//	lifetime
		glEnableVertexAttribArray(3);	//	lifespan

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(12));
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(24));
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(28));
	}
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void	GPUPointEmitter::CreateUpdateShader()
{
	if (m_uiUpdateShader != 0)
	{
		glDeleteProgram(m_uiUpdateShader);
	}

	unsigned int	uiVertexShader;
	LoadShaderType("./shaders/gpu_particle_update_vert.glsl", GL_VERTEX_SHADER, &uiVertexShader);

	m_uiUpdateShader = glCreateProgram();
	glAttachShader(m_uiUpdateShader, uiVertexShader);

	const char*	outputs[4] = { "updated_position", "updated_velocity", "updated_lifetime", "updated_lifespan"};

	glTransformFeedbackVaryings(m_uiUpdateShader, 4, outputs, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(m_uiUpdateShader);
	glDeleteShader(uiVertexShader);
}

void	GPUPointEmitter::CreateDrawShader()
{
	if (m_uiDrawShader != 0)
	{
		glDeleteProgram(m_uiDrawShader);
	}
	LoadShader("./shaders/gpu_particle_vertex.glsl", "./shaders/gpu_particle_geometry.glsl", "./shaders/gpu_particle_fragment.glsl", &m_uiDrawShader);
}

void	GPUPointEmitter::SetParticleTexture(unsigned int a_uiParticleTexture)
{
	m_uiParticleTexture = a_uiParticleTexture;
}

