
//	Download SourceTree to access GitHub:
//		www.sourcetreeapp.com
#include <cstdio>
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#define		GLM_SWIZZLE
#include "glm/glm.hpp"
#include "glm/ext.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

mat4	BuildOrbitMatrix(float timer, float radius, float orbitRotation)
{
	mat4	transform = glm::rotate(timer, vec3(0, 1, 0)) * glm::translate(vec3(radius, 0, 0)) * glm::rotate(orbitRotation, vec3(0, 1, 0));
	return transform;
}

int main()
{
	vec3	myVec3;
	myVec3.x;
	if (glfwInit() == false)
	{
		return -1;
	}

	GLFWwindow*	window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);
	if (window == nullptr)
	{
		return -2;
	}
	glfwMakeContextCurrent(window);
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)	//	must be after you make context current
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}
	int	major_version = ogl_GetMajorVersion();
	int	minor_version = ogl_GetMinorVersion();

	printf("Successfully loaded OpenGL version %d.%d!\n", major_version, minor_version);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();

	mat4	view = glm::lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4	projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
	float	camera_x = -10.0f;
	float	camera_z = -10.0f;
	float	timer = 0.0f;

	while (glfwWindowShouldClose(window) == false)
	{
		float	dT = glfwGetTime();
		glfwSetTime(0.0f);
		//	now we get to the fun stuff
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Gizmos::clear();
		Gizmos::addTransform(mat4(1));

		timer += dT;
		//	messes with the view
		camera_x = sinf(0.3f * timer) * 10;
		camera_z = cosf(0.2f * timer) * 10;

		mat4	view = glm::lookAt(vec3(camera_x, 10, camera_z), vec3(0, 0, 0), vec3(0, 1, 0));

		vec4	white(1);
		vec4	black(0, 0, 0, 1);
		vec4	blue(0, 0, 1, 1);
		vec4	yellow(1, 1, 0, 1);
		vec4	green(0, 1, 0, 1);
		vec4	red(1, 0, 0, 1);
		for (int i = 0; i <= 20; ++i)
		{
			Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
							i == 10? white : black);
			Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
							i == 10 ? white : black);

		}
		Gizmos::addTri(vec3(0, 1, 0), vec3(2, -1, -1), vec3(-3, -2, 4), blue);
		Gizmos::addAABB(vec3(2.5f, 5, 0), vec3(1, 1, 1), blue);
		Gizmos::addAABBFilled(vec3(-2.5f, 5, 0), vec3(1, 1, 1), green);

		//mat4	sun_transform = glm::rotate(timer * 0.25f, vec3(0, 1, 0));	//	rotation matrix
		mat4	sun_transform = BuildOrbitMatrix(timer * 0.25f, 0, 0);
		mat4	planet1 = sun_transform * BuildOrbitMatrix(timer, 7, timer * 0.5f);
		mat4	moon1 = planet1 * BuildOrbitMatrix(timer * 2, 2, timer * 5.0f);


		mat4	rotation = glm::rotate(timer, vec3(0, 1, 0));	//	rotation matrix
		mat4	day_rotation = glm::rotate(timer * 2.0f, vec3(0, 1, 0));	//	rotation matrix
		mat4	translation_matrix = glm::translate(vec3(8, 0, 0));	//	translation matrix
		mat4	translation2_matrix = glm::translate(vec3(3, 0, 0));	//	translation matrix
		mat4	transform = translation_matrix * rotation;
		mat4	child_one_transform = rotation * translation_matrix * day_rotation;
		mat4	child_two_transform = child_one_transform * rotation;
		//vec4	centre = transform * vec4(0, 0, 0, 1);
		Gizmos::addSphere(sun_transform[3].xyz, 3.0f, 20, 20, yellow, &sun_transform);
		Gizmos::addSphere(planet1[3].xyz, 1.0f, 20, 20, blue, &planet1);
		Gizmos::addSphere(moon1[3].xyz, 0.25f, 20, 20, green, &moon1);
		//		Gizmos::addSphere(child_one_transform[3].xyz, 1.0f, 20, 20, blue, &child_one_transform);
//		Gizmos::addSphere(child_two_transform[3].xyz, 1.0f, 20, 20, blue, &child_two_transform);



		Gizmos::draw(projection, view);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}





	//	now clean up
	Gizmos::destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
