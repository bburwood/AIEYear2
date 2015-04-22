#include "UtilitySystemApplication.h"
#include "gl_core_4_4.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Camera.h"
#include "Gizmos.h"

#include "World.h"
#include "BaseNPC.h"
#include "UtilityNPC.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

UtilitySystemApplication::UtilitySystemApplication()
	: m_camera(nullptr) {
}

UtilitySystemApplication::~UtilitySystemApplication() {

}

bool UtilitySystemApplication::startup() {

	// create a basic window
	createWindow("Utility Systems", 1280, 720);

	// start the gizmo system that can draw basic shapes
	Gizmos::create();

	// create a camera
	m_camera = new Camera(glm::pi<float>() * 0.25f, 16 / 9.f, 0.1f, 1000.f);
	m_camera->setLookAtFrom(vec3(0, 12, 22), vec3(0));
	
	m_pWorld = new World();
	//m_pNPC = new BaseNPC(m_pWorld);
	m_pNPC = new UtilitySystem::UtilityNPC(m_pWorld);

	return true;
}

void UtilitySystemApplication::shutdown() {

	//////////////////////////////////////////////////////////////////////////
	// YOUR SHUTDOWN CODE HERE
	//////////////////////////////////////////////////////////////////////////

	// delete our camera and cleanup gizmos
	delete m_camera;
	Gizmos::destroy();

	// destroy our window properly
	destroyWindow();
}

bool UtilitySystemApplication::update(float deltaTime) {
	
	// close the application if the window closes
	if (glfwWindowShouldClose(m_window) ||
		glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		return false;

	// update the camera's movement
	m_camera->update(deltaTime);

	m_pNPC->update(deltaTime);

	// clear the gizmos out for this frame
	Gizmos::clear();

	// ...for now let's add a grid to the gizmos
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10), vec3(-10 + i, 0, -10),
			i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));

		Gizmos::addLine(vec3(10, 0, -10 + i), vec3(-10, 0, -10 + i),
			i == 10 ? vec4(1, 1, 1, 1) : vec4(0, 0, 0, 1));
	}

	// return true, else the application closes
	return true;
}

void UtilitySystemApplication::draw() {

	// clear the screen for this frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	m_pWorld->render();
	m_pNPC->render();


	// display the 3D gizmos
	Gizmos::draw(m_camera->getProjectionView());

	// get a orthographic projection matrix and draw 2D gizmos
	int width = 0, height = 0;
	glfwGetWindowSize(m_window, &width, &height);
	mat4 guiMatrix = glm::ortho<float>(0, 0, (float)width, (float)height);

	Gizmos::draw2D(m_camera->getProjectionView());
}