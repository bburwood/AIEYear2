#include "Application.h"
#include "gl_core_4_4.h"
#include "GLFW/glfw3.h"
#include <cstdio>

Application::Application(){}
Application::~Application(){}

bool Application::startup()
{
	if (glfwInit() == false)
	{
		return false;
	}

	this->m_window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);
	if (this->m_window == nullptr)
	{
		return false;
	}

	glfwMakeContextCurrent(this->m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(this->m_window);
		glfwTerminate();
		return false;
	}

	int major_version = ogl_GetMajorVersion();
	int minor_version = ogl_GetMinorVersion();
	printf("Successfully loaded OpenGL version %d.%d\n",
		major_version, minor_version);


	glfwSwapInterval(1);


	return true;
}

void Application::shutdown()
{
	glfwDestroyWindow(this->m_window);
	glfwTerminate();
}

bool Application::update() 
{ 
	if ( glfwWindowShouldClose(m_window) )
	{
		return false;
	}
	
	return true; 
}

void Application::draw() {}