#include "PostProcess.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

const int	BUFFER_WIDTH = 1280;
const int	BUFFER_HEIGHT = 720;

PostProcess::PostProcess()
{
}
PostProcess::~PostProcess()
{
}

bool	PostProcess::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	m_fTotTime = 0.0f;

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);
	//	setup callbacks to send info to AntTweakBar
	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);

	m_BackgroundColour = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	//	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glfwSetTime(0.0f);

	Gizmos::create();
	m_bDrawGizmos = true;

	//	now initialise the FlyCamera
	m_FlyCamera = FlyCamera(vec3(10, 10, 10), vec3(0, 0, 0), glm::radians(50.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	//	initialise basic AntTweakBar info
	m_bar = TwNewBar("Amazing new AntTweakBar!!");
	TwAddSeparator(m_bar, "Light Data", "");
	//	TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_light_dir, "label='Group Light'");
	//	TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR3F, &m_light_colour, "label='Group Light'");
	//	TwAddVarRW(m_bar, "Spec Power", TW_TYPE_FLOAT, &m_fSpecular_power, "label='Group Light' min=0.05 max=100 step=0.05");
	TwAddSeparator(m_bar, "Misc Data", "");
	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_BackgroundColour, "");
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_bDrawGizmos, "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fFPS, "");

	//	create the frame buffer
	GenerateFrameBuffer();
	//	create the quad mesh
	GenerateScreenSpaceQuad();
	//	load the post effect shader
	LoadShader("shaders/post_vertex.glsl", 0, "shaders/post_fragment.glsl", &m_uiPostProgramID);

	return true;
}

void	PostProcess::shutdown()
{
	//	now clean up
	Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
}

bool	PostProcess::update()
{
	if (Application::update() == false)
	{
		return false;
	}
	//	check if we need to reload the shaders
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}

	float	dT = (float)glfwGetTime();
	glfwSetTime(0.0f);
	m_fFPS = (float)(1.0 / dT);
	m_fTotTime += dT;

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

	Gizmos::addSphere(vec3(0, 5, 0), 0.5f, 12, 12, vec4(1, 1, 0, 1));
	Gizmos::addSphere(vec3(2, 7, 3), 0.5f, 12, 12, blue);



	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	return true;
}

void	PostProcess::draw()
{
	Application::draw();

	//	bind frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_uifbo);
	glViewport(0, 0, BUFFER_WIDTH, BUFFER_HEIGHT);

	//	clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	render everything as normal, but to the frame buffer
	if (m_bDrawGizmos)
	{
		Gizmos::draw(m_FlyCamera.GetProjectionView());
	}

	//	bind the back buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, BUFFER_WIDTH, BUFFER_HEIGHT);

	//	just clear the back buffer depth as each pixel will be filled
	glClear(GL_DEPTH_BUFFER_BIT);

	//	bind post effect shader
	glUseProgram(m_uiPostProgramID);

	//	bind the fbo_texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uifboTexture);
	int	iTexLocation = glGetUniformLocation(m_uiPostProgramID, "input_texture");
	glUniform1i(iTexLocation, 0);
	int iTimeLocation = glGetUniformLocation(m_uiPostProgramID, "fTime");
	glUniform1f(iTimeLocation, m_fTotTime);

	//	render our quad with the texture on it
	glBindVertexArray(m_quad.m_uiVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	TwDraw();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void	PostProcess::GenerateFrameBuffer()
{
	glGenFramebuffers(1, &m_uifbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_uifbo);

	glGenTextures(1, &m_uifboTexture);
	glBindTexture(GL_TEXTURE_2D, m_uifboTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, BUFFER_WIDTH, BUFFER_HEIGHT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//	clamping the texture to the edge
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_uifboTexture, 0);

	glGenRenderbuffers(1, &m_uifboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_uifboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, BUFFER_WIDTH, BUFFER_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_uifboDepth);
	GLenum	drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Frame Buffer did not generate properly!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void	PostProcess::GenerateScreenSpaceQuad()
{
	vec2	halfTexel = 1.0f / (vec2((float)BUFFER_WIDTH, (float)BUFFER_HEIGHT) * 0.5f);
//	vec2	halfTexel = 1.0f / vec2(1280.0f / 720.0f);
	//	this is so that the UV's are sampled from teh correct texel.
	float	vertexData[]
	{
		-1, -1, 0, 1,		halfTexel.x, halfTexel.y,
		-1, 1, 0, 1,		halfTexel.x, 1.0f - halfTexel.y,
		1, 1, 0, 1,			1.0f - halfTexel.x, 1.0f - halfTexel.y,
		1, -1, 0, 1,		1.0f - halfTexel.x, halfTexel.y,
	};

	unsigned int	indexData[]
	{
		0, 1, 2, 0, 2, 3
	};

	glGenVertexArrays(1, &m_quad.m_uiVAO);
	glBindVertexArray(m_quad.m_uiVAO);

	glGenBuffers(1, &m_quad.m_uiVBO);
	glGenBuffers(1, &m_quad.m_uiIBO);


	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	tex coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	PostProcess::ReloadShader()
{
	m_fTotTime = 0.0f;
	glDeleteProgram(m_uiPostProgramID);
	LoadShader("shaders/post_vertex.glsl", 0, "shaders/post_fragment.glsl", &m_uiPostProgramID);
}
