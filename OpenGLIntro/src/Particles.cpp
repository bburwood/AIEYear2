#include "Particles.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

Particles::Particles()
{
}
Particles::~Particles()
{
}

bool	Particles::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();
	m_bDrawGizmos = true;

	//	now initialise the FlyCamera
	m_FlyCamera = FlyCamera(vec3(15, 15, 15), vec3(0, 6, 0), glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	m_BackgroundColour = vec4(0.3f, 0.3f, 0.3f, 1.0f);

	LoadShader("./shaders/particle_vertex.glsl", "./shaders/particle_fragment.glsl", &m_uiProgramID);
	m_emitter.Init(50000, vec3(0, 15, 0), 4000.0f, 0.5f, 2.0f, 0.4f, 4.0f, 2.0f, 0.01f, 0.04f, vec4(0.8, 1.0, 0.25, 1), vec4(1, 1, 0.8f, 1));
	return true;
}

void	Particles::shutdown()
{
	//	now clean up
	Gizmos::destroy();
}

bool	Particles::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	float	dT = (float)glfwGetTime();
	glfwSetTime(0.0f);
	//	now we get to the fun stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Gizmos::clear();
	Gizmos::addTransform(mat4(1));

	m_timer += dT;
	m_FlyCamera.update(dT);

	vec4	white(1);
	vec4	black(0, 0, 0, 1);
	vec4	blue(0, 0, 1, 1);
	vec4	yellow(1, 1, 0, 1);
	vec4	green(0, 1, 0, 1);
	vec4	red(1, 0, 0, 1);
	for (int i = 0; i <= 20; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? white : black);
	}

	if (glfwGetKey(m_window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		m_emitter.IncreaseEmitRate();
	}
	if (glfwGetKey(m_window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		m_emitter.DecreaseEmitRate();
	}

	m_emitter.Update(dT);
	m_emitter.UpdateVertexData(m_FlyCamera.m_worldTransform[3].xyz, m_FlyCamera.m_worldTransform[1].xyz);

	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	return true;
}

void	Particles::draw()
{
	Application::draw();
	if (m_bDrawGizmos)
	{
		Gizmos::draw(m_FlyCamera.GetProjectionView());
	}

	glUseProgram(m_uiProgramID);
	int	iViewProjUniform = glGetUniformLocation(m_uiProgramID, "projection_view");
	glUniformMatrix4fv(iViewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.GetProjectionView());
	m_emitter.Render();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

