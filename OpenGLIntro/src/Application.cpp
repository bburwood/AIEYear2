#include "Application.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include <cstdio>


Application::Application()
{
}

Application::~Application()
{
}

void setVSync(bool sync)
{
	// Function pointer for the wgl extention function we need to enable/disable
	// vsync
	typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

	char *extensions = (char*)glGetString(GL_EXTENSIONS);

	if (strstr(extensions, "WGL_EXT_swap_control") == 0)
	{
		return;
	}
	else
	{
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if (wglSwapIntervalEXT)
			wglSwapIntervalEXT(sync);
	}
}

bool	Application::startup()
{
	if (glfwInit() == false)
	{
		return -1;
	}

	this->m_window = glfwCreateWindow(1920, 1080, "Computer Graphics", nullptr, nullptr);
	if (m_window == nullptr)
	{
		return -2;
	}
	glfwMakeContextCurrent(this->m_window);
	//setVSync(1);  // 0 disable VSync, 1 enable vsync - needs glew
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)	//	must be after you make context current
	{
		glfwDestroyWindow(this->m_window);
		glfwTerminate();
		return -3;
	}
	int	major_version = ogl_GetMajorVersion();
	int	minor_version = ogl_GetMinorVersion();

	glfwSwapInterval(0);	//	turns vsync on or off, 0 for off, 1 for on.

	printf("Successfully loaded OpenGL version %d.%d!\n", major_version, minor_version);
	return true;
}

void	Application::shutdown()
{
	glfwDestroyWindow(this->m_window);
	glfwTerminate();
}

bool	Application::update()
{
	if (glfwWindowShouldClose(this->m_window) == true)
	{
		return false;
	}
	return true;
}

void	Application::draw()
{
}


