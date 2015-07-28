#include "CustomPhysics.h"
#include "Gizmos.h"
#include "Utility.h"
#include <iostream>

using namespace std;

//	define some basic colours to use for drawing
vec4	white(1);
vec4	black(0, 0, 0, 1);
vec4	blue(0, 0, 1, 1);
vec4	yellow(1, 1, 0, 1);
vec4	green(0, 1, 0, 1);
vec4	red(1, 0, 0, 1);

CustomPhysics::CustomPhysics()
{

}

CustomPhysics::~CustomPhysics()
{

}


bool	CustomPhysics::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	m_fTotalTime = 0.0f;

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(BUFFER_WIDTH, BUFFER_HEIGHT);
	//	setup callbacks to send info to AntTweakBar
	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);

	m_BackgroundColour = vec4(0.1f, 0.1f, 0.3f, 1.0f);
	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	//	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();
	m_bDrawGizmos = true;

	//	now initialise the FlyCamera
	m_FlyCamera = FlyCamera(vec3(-1, 10, 5), vec3(-1, 0, 1), glm::radians(50.0f), (float)BUFFER_WIDTH / (float)BUFFER_HEIGHT, 0.1f, 300.0f);
	m_FlyCamera.SetSpeed(25.0f);

	//	initialise basic AntTweakBar info
	m_bar = TwNewBar("GeneralStuff");	//	must be a single word (no spaces) if you want to be able to resize it
	TwDefine(" GeneralStuff size='320 600' "); // resize bar
	TwAddSeparator(m_bar, "Misc Data", "");
	TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_vLightDir, "label='Light Direction'");
	TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR4F, &m_vLightColour, "");
	TwAddVarRW(m_bar, "Ambient Colour", TW_TYPE_COLOR4F, &m_vAmbientLightColour, "");
	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_BackgroundColour, "");
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_bDrawGizmos, "");
	TwAddVarRW(m_bar, "Camera Speed", TW_TYPE_FLOAT, &m_FlyCamera.m_fSpeed, "min=1 max=250 step=1");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fFPS, "");


	m_vAmbientLightColour = vec4(0.001f, 0.001f, 0.001f, 1.0f);
	m_vLightColour = vec4(0.2f, 0.2f, 0.2f, 1.0f);
	m_vLightDir = glm::normalize(vec3(-0.10f, -0.85f, 0.5f));

	cout << "CustomPhysics startup complete!\n";
	return true;
}

void	CustomPhysics::shutdown()
{
	//	now clean up
	Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
}

bool	CustomPhysics::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	//	check if we need to reload the shaders
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		//ReloadShader();
	}
	float	dT = (float)glfwGetTime();
	glfwSetTime(0.0f);
	m_fFPS = (float)(1.0f / dT);
	m_FlyCamera.update(dT);



	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	return true;
}

void	CustomPhysics::draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Application::draw();
	Gizmos::clear();
	Gizmos::addTransform(mat4(1));	//	adds the red green blue unit vectors to the drawn grid of lines





	if (m_bDrawGizmos)
	{
		for (int i = 0; i <= 20; ++i)
		{
			Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
				i == 10 ? white : black);
			Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
				i == 10 ? white : black);
		}
		Gizmos::draw(m_FlyCamera.GetProjectionView());
	}
	TwDraw();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}




