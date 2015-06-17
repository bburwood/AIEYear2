#include "Application.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include <cstdio>
#include "Bitboard.h"

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
		return false;
	}

	this->m_window = glfwCreateWindow(BUFFER_WIDTH, BUFFER_HEIGHT, "Checkers Assignment", nullptr, nullptr);
	if (m_window == nullptr)
	{
		return false;
	}
	glfwMakeContextCurrent(this->m_window);
	//setVSync(1);  // 0 disable VSync, 1 enable vsync - needs glew
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)	//	must be after you make context current
	{
		glfwDestroyWindow(this->m_window);
		glfwTerminate();
		return false;
	}
	int	major_version = ogl_GetMajorVersion();
	int	minor_version = ogl_GetMinorVersion();

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
	if (glfwWindowShouldClose(this->m_window))
	{
		return false;
	}
	return true;
}

void	Application::draw()
{
}


