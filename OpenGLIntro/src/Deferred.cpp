#include "Deferred.h"
#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include "Gizmos.h"
#include "Utility.h"
#include "tiny_obj_loader.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

const int	BUFFER_WIDTH = 1280;
const int	BUFFER_HEIGHT = 720;

Deferred::Deferred()
{
}
Deferred::~Deferred()
{
}

bool	Deferred::startup()
{
	if (Application::startup() == false)
	{
		return false;
	}
	//	check if we need to reload the shaders
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}

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
	m_bDrawGizmos = false;

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

	BuildMeshes();
	BuildGBuffer();
	BuildLightBuffer();
	BuildQuad();

	LoadShader("shaders/gbuffer_vertex.glsl", 0, "shaders/gbuffer_fragment.glsl", &m_uiGBufferProgram);
	LoadShader("shaders/composite_vertex.glsl", 0, "shaders/composite_fragment.glsl", &m_uiCompositeProgram);
	LoadShader("shaders/composite_vertex.glsl", 0, "shaders/directional_light_fragment.glsl", &m_uiDirectionalLightProgram);

	return true;
}

void	Deferred::shutdown()
{
	//	now clean up
	Gizmos::destroy();
	TwDeleteAllBars();
	TwTerminate();
}

bool	Deferred::update()
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


	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	return true;
}

void	Deferred::draw()
{
	Application::draw();

	////////////////////////
	//	G-BUFFER generation
	////////////////////////
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_uiGBufferFBO);
	glClearColor(0, 0, 0, 0);	//	so we know where nothing was rendered

	glClear(GL_DEPTH_BUFFER_BIT);

	vec4	clearColour = vec4(0, 0, 0, 0);
	vec4	clearNormal = vec4(0.5f, 0.5f, 0.5f, 0.0f);

	glClearBufferfv(GL_COLOR, 0, (float*)&clearColour);
	glClearBufferfv(GL_COLOR, 1, (float*)&clearColour);
	glClearBufferfv(GL_COLOR, 2, (float*)&clearNormal);

	glUseProgram(m_uiGBufferProgram);

	int	iViewUniform = glGetUniformLocation(m_uiGBufferProgram, "view");
	int	iViewProjUniform = glGetUniformLocation(m_uiGBufferProgram, "view_proj");

	glUniformMatrix4fv(iViewUniform, 1, GL_FALSE, (float*)&m_FlyCamera.m_viewTransform);
	glUniformMatrix4fv(iViewProjUniform, 1, GL_FALSE, (float*)&m_FlyCamera.m_projectionViewTransform);

	glBindVertexArray(m_Bunny.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_Bunny.m_uiIndexCount, GL_UNSIGNED_INT, 0);

	////////////////////////
	//	Light accumulation
	////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiLightFBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(m_uiDirectionalLightProgram);
	int	iPositionTexUniform = glGetUniformLocation(m_uiDirectionalLightProgram, "position_tex");
	int	iNormalsTexUniform = glGetUniformLocation(m_uiDirectionalLightProgram, "normals_tex");
	glUniform1i(iPositionTexUniform, 0);
	glUniform1i(iNormalsTexUniform, 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiPositionsTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_uiNormalsTexture);

	vec3	light_dir = vec3(0, -1, 0);
	vec3	light_colour = vec3(0.2f, 0.4f, 0.8f);
	RenderDirectionalLight(light_dir, light_colour);
	RenderDirectionalLight(vec3(0, 1, 0), vec3(0.5f, 0.25f, 0.0f));
	RenderDirectionalLight(vec3(1, 1, 0), vec3(0.6f, 0.0f, 0.0f));
	RenderDirectionalLight(vec3(1, 0, 0), vec3(0.0f, 0.6f, 0.0f));
	RenderDirectionalLight(vec3(0, 0, 1), vec3(0.0f, 0.0f, 0.75f));
	RenderDirectionalLight(vec3(0.0f, 1.0f, 1.0f), vec3(0.4f, 0.4f, 0.4f));
	RenderDirectionalLight(vec3(-1.0f, -0.10f, -1.0f), vec3(0.5f, 0.5f, 0.0f));

	glDisable(GL_BLEND);

	////////////////////////
	//	Composite pass
	////////////////////////
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glClearColor(m_BackgroundColour.x, m_BackgroundColour.y, m_BackgroundColour.z, m_BackgroundColour.w);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_uiCompositeProgram);

	int	iAlbedoTexUniform = glGetUniformLocation(m_uiCompositeProgram, "albedo_tex");
	int	iLightTexUniform = glGetUniformLocation(m_uiCompositeProgram, "light_tex");

	glUniform1i(iAlbedoTexUniform, 0);
	glUniform1i(iLightTexUniform, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uiAlbedoTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_uiLightTexture);

	/*
	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	*/

	glBindVertexArray(m_ScreenSpaceQuad.m_uiVAO);
	glDrawElements(GL_TRIANGLES, m_ScreenSpaceQuad.m_uiIndexCount, GL_UNSIGNED_INT, 0);



	if (m_bDrawGizmos)
	{
		Gizmos::draw(m_FlyCamera.GetProjectionView());
	}

	TwDraw();
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void	Deferred::BuildMeshes()
{
	std::vector<tinyobj::shape_t>	shapes;
	std::vector<tinyobj::material_t>	materials;

	tinyobj::LoadObj(shapes, materials, "./models/stanford/bunny.obj");

	m_Bunny.m_uiIndexCount = shapes[0].mesh.indices.size();

	tinyobj::mesh_t*	mesh = &shapes[0].mesh;

	std::vector<float>	vertexData;
	vertexData.reserve(mesh->positions.size() + mesh->normals.size());

	vertexData.insert(vertexData.end(), mesh->positions.begin(), mesh->positions.end());
	vertexData.insert(vertexData.end(), mesh->normals.begin(), mesh->normals.end());

	glGenVertexArrays(1, &m_Bunny.m_uiVAO);
	glBindVertexArray(m_Bunny.m_uiVAO);

	glGenBuffers(1, &m_Bunny.m_uiVBO);
	glGenBuffers(1, &m_Bunny.m_uiIBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_Bunny.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Bunny.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* vertexData.size(), vertexData.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	normal
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)* mesh->positions.size()));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

/*
	//	plane
	glGenVertexArrays(1, &m_plane.m_uiVAO);
	glBindVertexArray(m_plane.m_uiVAO);
	glGenBuffers(1, &m_plane.m_uiVBO);
	glGenBuffers(1, &m_plane.m_uiIBO);

	float	planeVertexData[] =
	{
		-10.0f, 0.0f, -10.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		10.0f, 0.0f, -10.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		10.0f, 0.0f, 10.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		-10.0f, 0.0f, 10.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	};
	unsigned int	planeIndexData[] =
	{
		0, 1, 2, 0, 2, 3
	};
	m_plane.m_uiIndexCount = 6;

	glBindBuffer(GL_ARRAY_BUFFER, m_plane.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertexData), planeVertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndexData), planeIndexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 8, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 8, (void*)(sizeof(float)* 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
*/
}

void	Deferred::BuildGBuffer()
{
	//	create the framebuffer
	glGenFramebuffers(1, &m_uiGBufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiGBufferFBO);

	//	generate all our textures
	//	albedo, position, normal, depthI(depth is render buffer)
	glGenTextures(1, &m_uiAlbedoTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiAlbedoTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &m_uiPositionsTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiPositionsTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &m_uiNormalsTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiNormalsTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &m_uiGBufferDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_uiGBufferDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);

	//	attach our textures to the framebuffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_uiAlbedoTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_uiPositionsTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_uiNormalsTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_uiGBufferDepth);

	GLenum	targets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};

	glDrawBuffers(3, targets);

	//	check that it worked
	GLenum	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Aahhhh!!  It broke!!  G-Buffer creation FAIL!!\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void	Deferred::BuildLightBuffer()
{
	//	create the fbo
	glGenFramebuffers(1, &m_uiLightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiLightFBO);

	//	create textures
	//	just the light texture this time
	glGenTextures(1, &m_uiLightTexture);
	glBindTexture(GL_TEXTURE_2D, m_uiLightTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//	attach textures
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_uiLightTexture, 0);
	GLenum	lightTarget = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &lightTarget);

	//	check framebuffer for success
	GLenum	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Aahhhh!!  It broke!!  Light Buffer creation FAIL!!\n");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void	Deferred::BuildQuad()
{
	vec2	halfTexel = 1.0f / (vec2((float)BUFFER_WIDTH, (float)BUFFER_HEIGHT) * 0.5f);
	//	vec2	halfTexel = 1.0f / vec2(1280.0f / 720.0f);
	//	this is so that the UV's are sampled from teh correct texel.
	float	vertexData[]
	{
		-1, -1, 0, 1, halfTexel.x, halfTexel.y,
			-1, 1, 0, 1, halfTexel.x, 1.0f - halfTexel.y,
			1, 1, 0, 1, 1.0f - halfTexel.x, 1.0f - halfTexel.y,
			1, -1, 0, 1, 1.0f - halfTexel.x, halfTexel.y,
	};

	unsigned int	indexData[]
	{
		0, 1, 2, 0, 2, 3
	};

	m_ScreenSpaceQuad.m_uiIndexCount = 6;

	glGenVertexArrays(1, &m_ScreenSpaceQuad.m_uiVAO);
	glBindVertexArray(m_ScreenSpaceQuad.m_uiVAO);

	glGenBuffers(1, &m_ScreenSpaceQuad.m_uiVBO);
	glGenBuffers(1, &m_ScreenSpaceQuad.m_uiIBO);


	glBindBuffer(GL_ARRAY_BUFFER, m_ScreenSpaceQuad.m_uiVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ScreenSpaceQuad.m_uiIBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexData), indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);	//	position
	glEnableVertexAttribArray(1);	//	tex coord

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, (void*)(sizeof(float)* 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void	Deferred::RenderDirectionalLight(vec3 light_dir, vec3 light_colour)
{
	vec4	viewspace_light_dir = m_FlyCamera.m_viewTransform * vec4(glm::normalize(light_dir), 0);
	int iLightDirUniform = glGetUniformLocation(m_uiDirectionalLightProgram, "light_dir");
	int iLightColourUniform = glGetUniformLocation(m_uiDirectionalLightProgram, "light_colour");

	glUniform3fv(iLightDirUniform, 1, (float*)&viewspace_light_dir);
	glUniform3fv(iLightColourUniform, 1, (float*)&light_colour);

	glBindVertexArray(m_ScreenSpaceQuad.m_uiVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void	Deferred::ReloadShader()
{
	glDeleteProgram(m_uiGBufferProgram);
	glDeleteProgram(m_uiCompositeProgram);
	glDeleteProgram(m_uiDirectionalLightProgram);
	LoadShader("shaders/gbuffer_vertex.glsl", 0, "shaders/gbuffer_fragment.glsl", &m_uiGBufferProgram);
	LoadShader("shaders/composite_vertex.glsl", 0, "shaders/composite_fragment.glsl", &m_uiCompositeProgram);
	LoadShader("shaders/composite_vertex.glsl", 0, "shaders/directional_light_fragment.glsl", &m_uiDirectionalLightProgram);
}
