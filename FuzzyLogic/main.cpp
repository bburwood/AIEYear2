// a blank OpenGL window example

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "FuzzyLogicProject.h"
#include <stdio.h>

// callback for GL debug messages
void __stdcall logGLDebugCallback(unsigned int a_source,
	unsigned int a_type,
	unsigned int a_id,
	unsigned int a_severity,
	int a_length,
	const char* a_message,
	const void* a_userParam);

int main(int argc, char* argv[])
{	
	// window creation and OpenGL initialisaion
	if (!glfwInit())
	{
		printf("GLFW initialisation failed\n");
		return -1;
	}

	// specify preffered GL version, enable the core profile only with forward compatibility
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// GL added a debug callback to eliminate the need to query for errors
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Example OpenGL Window", nullptr, nullptr);
	if (window == nullptr)
	{
		printf("Window creation failed\n");
		glfwTerminate();
		return -1;
	}

	// set the window as the current GL context for rendering in to
	glfwMakeContextCurrent(window);

	// update opengl function pointers using "OpenGL Loader Generator"
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		printf("GL function mapping failed\n");
		glfwTerminate();
		return -1;
	}

	// output active GL version
	printf("GL %i.%i\n", ogl_GetMajorVersion(), ogl_GetMinorVersion());

	// now that opengl is initialised we can set the debug callback for errors
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(logGLDebugCallback, nullptr);

	// set the window resize callback to reset our viewport size (using a lambda)
	glfwSetWindowSizeCallback(window, 
		[](GLFWwindow*, int w, int h){ glViewport(0,0,w,h); });

	// create our application
    FuzzyProject* application = new FuzzyProject();
	if (application->onCreate(argc, argv) == false)
	{
		printf("Application creation failed\n");
		glfwTerminate();
		return -1;
	}

	// something to track frames-per-second
	float prevTime = (float)glfwGetTime();

	// loop while window is open and escape hasn't been pressed
	while (!glfwWindowShouldClose(window)) 
	{
		// grab the time since the application started (in seconds)
		float time = (float)glfwGetTime();

		// calculate a delta time
		float deltaTime = time - prevTime;
		prevTime = time;

		// update and draw the application
		application->onUpdate(deltaTime);
		application->onDraw();

		// present the back-buffer
		glfwSwapBuffers(window);

		// poll the window for events (i.e. input, window resize)
		glfwPollEvents();
	}

	// cleanup
	application->onDestroy();
	delete application;

	// cleanup our window
	glfwTerminate();

	return 0;
}

void __stdcall logGLDebugCallback(unsigned int a_source,
	unsigned int a_type,
	unsigned int a_id,
	unsigned int a_severity,
	int a_length,
	const char* a_message,
	const void* a_userParam)
{
    if (a_type == GL_DEBUG_TYPE_OTHER)
    {
        return;
    }
	char source[16], type[20];
	if (a_source == GL_DEBUG_SOURCE_API)
		strcpy(source, "OpenGL");
	else if (a_source == GL_DEBUG_SOURCE_WINDOW_SYSTEM)
		strcpy(source, "Windows");
	else if (a_source == GL_DEBUG_SOURCE_SHADER_COMPILER)
		strcpy(source, "Shader Compiler");
	else if (a_source == GL_DEBUG_SOURCE_THIRD_PARTY)
		strcpy(source, "Third Party");
	else if (a_source == GL_DEBUG_SOURCE_APPLICATION)
		strcpy(source, "Application");
	else if (a_source == GL_DEBUG_SOURCE_OTHER)
		strcpy(source, "Other");

	if (a_type == GL_DEBUG_TYPE_ERROR)
		strcpy(type, "Error");
	else if (a_type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR)
		strcpy(type, "Deprecated Behavior");
	else if (a_type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
		strcpy(type, "Undefined Behavior");
	else if (a_type == GL_DEBUG_TYPE_PORTABILITY)
		strcpy(type, "Portability");
	else if (a_type == GL_DEBUG_TYPE_PERFORMANCE)
		strcpy(type, "Performance");
	else if (a_type == GL_DEBUG_TYPE_MARKER)
		strcpy(type, "Marker");
	else if (a_type == GL_DEBUG_TYPE_PUSH_GROUP)
		strcpy(type, "Push Group");
	else if (a_type == GL_DEBUG_TYPE_POP_GROUP)
		strcpy(type, "Pop Group");
	else if (a_type == GL_DEBUG_TYPE_OTHER)
		strcpy(type, "Other");

	if (a_severity == GL_DEBUG_SEVERITY_HIGH)
		printf("GL Error[ HIGH ]: %d\n\tType: %s\n\tSource: %s\n\tMessage: %s\n", a_id, type, source, a_message);
	else if (a_severity == GL_DEBUG_SEVERITY_MEDIUM)
		printf("GL Error[ MED ]: %d\n\tType: %s\n\tSource: %s\n\tMessage: %s\n", a_id, type, source, a_message);
	else if (a_severity == GL_DEBUG_SEVERITY_LOW)
		printf("GL Error[ LOW ]: %d\n\tType: %s\n\tSource: %s\n\tMessage: %s\n", a_id, type, source, a_message);
	else if (a_severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		printf("GL Message: %d\n\tType: %s\n\tSource: %s\n\tMessage: %s\n", a_id, type, source, a_message);
}