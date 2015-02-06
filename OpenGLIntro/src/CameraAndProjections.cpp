#include "CameraAndProjections.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

CameraAndProjections::CameraAndProjections()
{
}
CameraAndProjections::~CameraAndProjections()
{
}

bool	CameraAndProjections::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();

	//	now initialise the FlyCamera
	m_FlyCamera = FlyCamera(vec3(10, 10, 10), vec3(0, 0, 0), glm::radians(60.0f), 1280.0f/720.0f, 0.1f, 1000.0f);

}

void	CameraAndProjections::shutdown()
{
	//	now clean up
	Gizmos::destroy();
}

bool	CameraAndProjections::update()
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
	//	messes with the view
	//m_camera_x = sinf(0.3f * m_timer) * 10;
	//m_camera_z = cosf(0.2f * m_timer) * 10;
	//
	//m_view = glm::lookAt(vec3(m_camera_x, 10, m_camera_z), vec3(0, 0, 0), vec3(0, 1, 0));
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




	return true;
}

void	CameraAndProjections::draw()
{
//	Gizmos::draw(m_FlyCamera.m_projectionTransform, m_FlyCamera.m_viewTransform);
	Gizmos::draw(m_FlyCamera.GetProjectionView());
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

