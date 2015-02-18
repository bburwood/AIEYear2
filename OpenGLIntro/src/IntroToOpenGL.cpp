#include "IntroToOpenGL.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

IntroToOpenGL::IntroToOpenGL()
{
}
IntroToOpenGL::~IntroToOpenGL()
{
}

bool	IntroToOpenGL::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();

	m_view = glm::lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	m_camera_x = -10.0f;
	m_camera_z = -10.0f;
	m_timer = 0.0f;

	return true;
}

void	IntroToOpenGL::shutdown()
{
	//	now clean up
	Gizmos::destroy();
}

mat4	BuildOrbitMatrix(float orbitSpeed, float radius, float planetRotation)
{
	mat4	transform = glm::rotate(orbitSpeed, vec3(0, 1, 0)) * glm::translate(vec3(radius, 0, 0)) * glm::rotate(planetRotation, vec3(0, 1, 0));
	return transform;
}

bool	IntroToOpenGL::update()
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
	m_camera_x = sinf(0.3f * m_timer) * 10;
	m_camera_z = cosf(0.2f * m_timer) * 10;

	m_view = glm::lookAt(vec3(m_camera_x, 10, m_camera_z), vec3(0, 0, 0), vec3(0, 1, 0));

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
	Gizmos::addTri(vec3(0, 1, 0), vec3(2, -1, -1), vec3(-3, -2, 4), blue);
	Gizmos::addAABB(vec3(2.5f, 5, 0), vec3(1, 1, 1), blue);
	Gizmos::addAABBFilled(vec3(-2.5f, 5, 0), vec3(1, 1, 1), green);

	//mat4	sun_transform = glm::rotate(timer * 0.25f, vec3(0, 1, 0));	//	rotation matrix
	mat4	sun_transform = BuildOrbitMatrix(m_timer * 0.25f, 1, m_timer * 10.10f);
	mat4	planet1 = sun_transform * BuildOrbitMatrix(m_timer * 0.5f, 7, m_timer * 2.5f);
	mat4	moon1 = planet1 * BuildOrbitMatrix(m_timer * 2.0f, 2, m_timer * 3.0f);
	mat4	moon1moon1 = moon1 * BuildOrbitMatrix(m_timer * 1.5f, 0.35f, m_timer * 2.5f);
	mat4	moon1moon2 = moon1moon1 * BuildOrbitMatrix(m_timer * 1.0f, 0.25f, m_timer * 2.0f);


	mat4	rotation = glm::rotate(m_timer, vec3(0, 1, 0));	//	rotation matrix
	mat4	day_rotation = glm::rotate(m_timer * 2.0f, vec3(0, 1, 0));	//	rotation matrix
	mat4	translation_matrix = glm::translate(vec3(8, 0, 0));	//	translation matrix
	mat4	translation2_matrix = glm::translate(vec3(3, 0, 0));	//	translation matrix
	mat4	transform = translation_matrix * rotation;
	mat4	child_one_transform = rotation * translation_matrix * day_rotation;
	mat4	child_two_transform = child_one_transform * rotation;
	//vec4	centre = transform * vec4(0, 0, 0, 1);
	Gizmos::addSphere(sun_transform[3].xyz, 0.5f, 20, 20, yellow, &sun_transform);
	Gizmos::addSphere(planet1[3].xyz, 0.5f, 20, 20, blue, &planet1);
	Gizmos::addSphere(moon1[3].xyz, 0.15f, 15, 15, green, &moon1);
	Gizmos::addSphere(moon1moon1[3].xyz, 0.05f, 10, 10, green, &moon1moon1);
	Gizmos::addSphere(moon1moon2[3].xyz, 0.03f, 10, 10, blue, &moon1moon2);
	//		Gizmos::addSphere(child_one_transform[3].xyz, 1.0f, 20, 20, blue, &child_one_transform);
	//		Gizmos::addSphere(child_two_transform[3].xyz, 1.0f, 20, 20, blue, &child_two_transform);


	return true;
}

void	IntroToOpenGL::draw()
{
	Gizmos::draw(m_projection, m_view);
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}
